if(NOT APPLE)
    message(FATAL_ERROR "A universal macOS package can only be assembled on macOS")
endif()

foreach(REQUIRED_VARIABLE SOURCE_DIR ARM64_BINARY X86_64_BINARY OUTPUT_DIR)
    if(NOT DEFINED ${REQUIRED_VARIABLE} OR "${${REQUIRED_VARIABLE}}" STREQUAL "")
        message(FATAL_ERROR "${REQUIRED_VARIABLE} is required")
    endif()
endforeach()

find_program(LIPO_EXECUTABLE lipo REQUIRED)
find_program(OTOOL_EXECUTABLE otool REQUIRED)

function(require_architecture BINARY ARCHITECTURE)
    execute_process(
        COMMAND "${LIPO_EXECUTABLE}" -archs "${BINARY}"
        RESULT_VARIABLE LIPO_RESULT
        OUTPUT_VARIABLE BINARY_ARCHITECTURES
        ERROR_VARIABLE LIPO_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT LIPO_RESULT EQUAL 0 OR NOT BINARY_ARCHITECTURES MATCHES "(^| )${ARCHITECTURE}( |$)")
        message(FATAL_ERROR "${BINARY} does not contain ${ARCHITECTURE}: ${LIPO_ERROR}")
    endif()
endfunction()

function(read_dependencies BINARY OUTPUT_VARIABLE)
    execute_process(
        COMMAND "${OTOOL_EXECUTABLE}" -L "${BINARY}"
        RESULT_VARIABLE OTOOL_RESULT
        OUTPUT_VARIABLE OTOOL_OUTPUT
        ERROR_VARIABLE OTOOL_ERROR)
    if(NOT OTOOL_RESULT EQUAL 0)
        message(FATAL_ERROR "Unable to inspect ${BINARY}: ${OTOOL_ERROR}")
    endif()
    string(REGEX REPLACE "^[^\n]*\n" "" OTOOL_OUTPUT "${OTOOL_OUTPUT}")
    set(${OUTPUT_VARIABLE} "${OTOOL_OUTPUT}" PARENT_SCOPE)
endfunction()

require_architecture("${ARM64_BINARY}" arm64)
require_architecture("${X86_64_BINARY}" x86_64)
read_dependencies("${ARM64_BINARY}" ARM64_DEPENDENCIES)
read_dependencies("${X86_64_BINARY}" X86_64_DEPENDENCIES)
if(NOT "${ARM64_DEPENDENCIES}" STREQUAL "${X86_64_DEPENDENCIES}")
    message(FATAL_ERROR "The architecture slices use different runtime dependencies; rebuild both against the same universal dependency installation")
endif()

string(REPLACE "\n" ";" DEPENDENCY_LINES "${ARM64_DEPENDENCIES}")
foreach(DEPENDENCY_LINE IN LISTS DEPENDENCY_LINES)
    string(STRIP "${DEPENDENCY_LINE}" DEPENDENCY_LINE)
    if(DEPENDENCY_LINE STREQUAL "")
        continue()
    endif()
    string(REGEX REPLACE "^[^ ]+" "" DEPENDENCY_TRAILER "${DEPENDENCY_LINE}")
    string(REGEX REPLACE "^[ ]*" "" DEPENDENCY_TRAILER "${DEPENDENCY_TRAILER}")
    string(REPLACE "${DEPENDENCY_TRAILER}" "" DEPENDENCY_PATH "${DEPENDENCY_LINE}")
    if(DEPENDENCY_PATH MATCHES "^/usr/lib/" OR DEPENDENCY_PATH MATCHES "^@")
        continue()
    endif()
    if(NOT IS_ABSOLUTE "${DEPENDENCY_PATH}" OR NOT EXISTS "${DEPENDENCY_PATH}")
        message(FATAL_ERROR "Cannot verify universal runtime dependency ${DEPENDENCY_PATH}")
    endif()
    require_architecture("${DEPENDENCY_PATH}" arm64)
    require_architecture("${DEPENDENCY_PATH}" x86_64)
endforeach()

set(PACKAGE_ROOT "${OUTPUT_DIR}/bootgen-universal")
if(EXISTS "${PACKAGE_ROOT}")
    message(FATAL_ERROR "Refusing to overwrite existing package directory ${PACKAGE_ROOT}")
endif()
file(MAKE_DIRECTORY "${PACKAGE_ROOT}/bin" "${PACKAGE_ROOT}/share/doc/bootgen")
execute_process(
    COMMAND "${LIPO_EXECUTABLE}" -create "${ARM64_BINARY}" "${X86_64_BINARY}" -output "${PACKAGE_ROOT}/bin/bootgen"
    RESULT_VARIABLE CREATE_RESULT
    ERROR_VARIABLE CREATE_ERROR)
if(NOT CREATE_RESULT EQUAL 0)
    message(FATAL_ERROR "Unable to create universal Bootgen executable: ${CREATE_ERROR}")
endif()
require_architecture("${PACKAGE_ROOT}/bin/bootgen" arm64)
require_architecture("${PACKAGE_ROOT}/bin/bootgen" x86_64)
file(COPY "${SOURCE_DIR}/LICENSE" "${SOURCE_DIR}/README.md" "${SOURCE_DIR}/INSTALL.txt" "${SOURCE_DIR}/BUILDING.md" "${SOURCE_DIR}/THIRD_PARTY_NOTICES.md"
    DESTINATION "${PACKAGE_ROOT}/share/doc/bootgen")
file(COPY "${SOURCE_DIR}/lms-hash-sigs/license.txt" DESTINATION "${PACKAGE_ROOT}/share/doc/bootgen")
file(RENAME "${PACKAGE_ROOT}/share/doc/bootgen/license.txt" "${PACKAGE_ROOT}/share/doc/bootgen/LMS-HSS-LICENSE.txt")
message(STATUS "Created universal macOS staging tree: ${PACKAGE_ROOT}")
