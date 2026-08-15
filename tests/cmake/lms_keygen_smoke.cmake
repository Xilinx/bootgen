if(NOT DEFINED BOOTGEN OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "BOOTGEN and OUTPUT_DIR are required")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIR}")
file(CHMOD "${OUTPUT_DIR}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
set(PRIMARY_KEY "${OUTPUT_DIR}/primary.key")
set(SECONDARY_KEY "${OUTPUT_DIR}/secondary.key")
function(cleanup_test_keys)
    file(REMOVE
        "${PRIMARY_KEY}.prv" "${PRIMARY_KEY}.pub"
        "${SECONDARY_KEY}.prv" "${SECONDARY_KEY}.pub")
endfunction()

file(WRITE "${OUTPUT_DIR}/keygen.bif" "all:\n{\n    lms_key_params\n    {\n        primary {lms_shake256_h5_w2}\n        secondary {lms_shake256_h5_w2}\n    }\n    [pskfile] primary.key\n    [sskfile] secondary.key\n}\n")
execute_process(
    COMMAND "${BOOTGEN}" -arch versal_2ve_2vm -image "${OUTPUT_DIR}/keygen.bif" -generate_keys lms
    WORKING_DIRECTORY "${OUTPUT_DIR}"
    RESULT_VARIABLE KEYGEN_RESULT
    OUTPUT_VARIABLE KEYGEN_OUTPUT
    ERROR_VARIABLE KEYGEN_ERROR)
if(NOT KEYGEN_RESULT EQUAL 0 OR NOT EXISTS "${PRIMARY_KEY}.prv" OR NOT EXISTS "${PRIMARY_KEY}.pub" OR NOT EXISTS "${SECONDARY_KEY}.prv" OR NOT EXISTS "${SECONDARY_KEY}.pub")
    cleanup_test_keys()
    message(FATAL_ERROR "LMS key generation failed (${KEYGEN_RESULT}):\n${KEYGEN_OUTPUT}\n${KEYGEN_ERROR}")
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(STAT_COMMAND /usr/bin/stat -f %Lp)
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(STAT_COMMAND /usr/bin/stat -c %a)
endif()
if(DEFINED STAT_COMMAND)
    foreach(PRIVATE_KEY "${PRIMARY_KEY}.prv" "${SECONDARY_KEY}.prv")
        execute_process(
            COMMAND ${STAT_COMMAND} "${PRIVATE_KEY}"
            RESULT_VARIABLE STAT_RESULT
            OUTPUT_VARIABLE PRIVATE_KEY_MODE
            ERROR_VARIABLE STAT_ERROR
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(NOT STAT_RESULT EQUAL 0 OR NOT PRIVATE_KEY_MODE STREQUAL "600")
            cleanup_test_keys()
            message(FATAL_ERROR "LMS private key ${PRIVATE_KEY} must have mode 600 (got ${PRIVATE_KEY_MODE}): ${STAT_ERROR}")
        endif()
    endforeach()
endif()

cleanup_test_keys()
