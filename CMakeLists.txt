# Copyright 2015-2026 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20)

if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "Minimum supported macOS version" FORCE)
endif()

project(bootgen LANGUAGES C CXX)

option(BOOTGEN_BUILD_TESTS "Build Bootgen smoke tests" ON)
option(BOOTGEN_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(BOOTGEN_REGENERATE_PARSERS "Enable the parser regeneration maintainer target" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)
find_package(Threads REQUIRED)

set(BOOTGEN_INCLUDE_DIRS
    bisonflex
    common/include
    spartanup/include
    versal/include
    versal_2ve_2vm/include
    zynq/include
    zynqmp/include
    utils/include
    lms-hash-sigs
    win_include)

set(BOOTGEN_PARSER_SOURCES
    bisonflex/bif.tab.cpp
    bisonflex/bif.yy.cpp
    bisonflex/reginit.tab.cpp
    bisonflex/reginit.yy.cpp
    bisonflex/cmdoptions.tab.cpp
    bisonflex/cmdoptions.yy.cpp)

if(BOOTGEN_REGENERATE_PARSERS)
    find_package(BISON 2.7 REQUIRED)
    find_package(FLEX 2.5.35 REQUIRED)

    add_custom_target(regenerate-parsers
        COMMAND ${BISON_EXECUTABLE} --verbose
            --defines=${CMAKE_CURRENT_SOURCE_DIR}/bisonflex/bif.tab.hpp
            --output=${CMAKE_CURRENT_SOURCE_DIR}/bisonflex/bif.tab.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/bif.y
        COMMAND ${FLEX_EXECUTABLE} --prefix=bif
            --outfile=${CMAKE_CURRENT_SOURCE_DIR}/bisonflex/bif.yy.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/bif.l
        COMMAND ${BISON_EXECUTABLE} --verbose
            --defines=${CMAKE_CURRENT_SOURCE_DIR}/bisonflex/reginit.tab.hpp
            --output=${CMAKE_CURRENT_SOURCE_DIR}/bisonflex/reginit.tab.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/reginit.y
        COMMAND ${FLEX_EXECUTABLE} --prefix=reginit
            --outfile=${CMAKE_CURRENT_SOURCE_DIR}/bisonflex/reginit.yy.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/reginit.l
        COMMAND ${BISON_EXECUTABLE} --verbose
            --defines=${CMAKE_CURRENT_SOURCE_DIR}/bisonflex/cmdoptions.tab.hpp
            --output=${CMAKE_CURRENT_SOURCE_DIR}/bisonflex/cmdoptions.tab.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/cmdoptions.y
        COMMAND ${FLEX_EXECUTABLE} --prefix=cmdoptions
            --outfile=${CMAKE_CURRENT_SOURCE_DIR}/bisonflex/cmdoptions.yy.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/parser/cmdoptions.l
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Regenerating checked-in Bison and Flex parser sources"
        VERBATIM)
endif()

set(BOOTGEN_SOURCES
    common/src/Keccak-compact.cpp
    common/src/authentication.cpp
    common/src/authkeys.cpp
    common/src/bifoptions.cpp
    common/src/binary.cpp
    common/src/binfile.cpp
    common/src/bitutils.cpp
    common/src/bootheader.cpp
    common/src/bootimage.cpp
    common/src/checksum.cpp
    common/src/encryption.cpp
    common/src/hash.cpp
    common/src/imageheadertable.cpp
    common/src/kdf.cpp
    common/src/logger.cpp
    common/src/main.cpp
    common/src/mcsfile.cpp
    common/src/options.cpp
    common/src/outputfile.cpp
    common/src/parsing.cpp
    common/src/partition.cpp
    common/src/partitionheadertable.cpp
    common/src/postprocess.cpp
    common/src/readimage.cpp
    common/src/reginit.cpp
    spartanup/src/Keccak-compact-spartanup.cpp
    spartanup/src/authentication-spartanup.cpp
    spartanup/src/authkeys-spartanup.cpp
    spartanup/src/binary-spartanup.cpp
    spartanup/src/bitutils-spartanup.cpp
    spartanup/src/bootheader-spartanup.cpp
    spartanup/src/bootimage-spartanup.cpp
    spartanup/src/checksum-spartanup.cpp
    spartanup/src/encryption-spartanup.cpp
    spartanup/src/imageheadertable-spartanup.cpp
    spartanup/src/outputfile-spartanup.cpp
    spartanup/src/partition-spartanup.cpp
    spartanup/src/partitionheadertable-spartanup.cpp
    spartanup/src/readimage-spartanup.cpp
    spartanup/src/verifyimage-spartanup.cpp
    utils/src/cJSON.c
    utils/src/cdo-alloc.c
    utils/src/cdo-binary.c
    utils/src/cdo-command.c
    utils/src/cdo-load.c
    utils/src/cdo-metadata.c
    utils/src/cdo-npi.c
    utils/src/cdo-overlay.c
    utils/src/cdo-raw.c
    utils/src/cdo-source.c
    utils/src/elftools.cpp
    utils/src/encryptutils.cpp
    utils/src/fips202.c
    versal/src/Keccak-compact-versal.cpp
    versal/src/authentication-versal.cpp
    versal/src/authkeys-versal.cpp
    versal/src/binary-versal.cpp
    versal/src/bitutils-versal.cpp
    versal/src/bootheader-versal.cpp
    versal/src/bootimage-versal.cpp
    versal/src/checksum-versal.cpp
    versal/src/encryption-versal.cpp
    versal/src/encryption-versalnet.cpp
    versal/src/imageheadertable-versal.cpp
    versal/src/outputfile-versal.cpp
    versal/src/partition-versal.cpp
    versal/src/partitionheadertable-versal.cpp
    versal/src/readimage-versal.cpp
    versal/src/verifyimage-versal.cpp
    versal_2ve_2vm/src/Keccak-compact-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/authentication-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/authkeys-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/binary-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/bitutils-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/bootheader-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/bootimage-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/checksum-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/encryption-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/imageheadertable-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/outputfile-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/partition-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/partitionheadertable-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/postprocess_versal_2ve_2vm.cpp
    versal_2ve_2vm/src/readimage-versal_2ve_2vm.cpp
    versal_2ve_2vm/src/tlbingeneration.cpp
    versal_2ve_2vm/src/verifyimage-versal_2ve_2vm.cpp
    zynq/src/authentication-zynq.cpp
    zynq/src/bootheader-zynq.cpp
    zynq/src/bootimage-zynq.cpp
    zynq/src/encryption-zynq.cpp
    zynq/src/imageheadertable-zynq.cpp
    zynq/src/partitionheadertable-zynq.cpp
    zynq/src/readimage-zynq.cpp
    zynqmp/src/authentication-zynqmp.cpp
    zynqmp/src/bootheader-zynqmp.cpp
    zynqmp/src/bootimage-zynqmp.cpp
    zynqmp/src/encryption-zynqmp.cpp
    zynqmp/src/imageheadertable-zynqmp.cpp
    zynqmp/src/partitionheadertable-zynqmp.cpp
    zynqmp/src/readimage-zynqmp.cpp
    zynqmp/src/verifyimage-zynqmp.cpp)

set(LMS_HASH_SIGS_SOURCES
    lms-hash-sigs/endian.c
    lms-hash-sigs/fips202.c
    lms-hash-sigs/hss.c
    lms-hash-sigs/hss_alloc.c
    lms-hash-sigs/hss_aux.c
    lms-hash-sigs/hss_common.c
    lms-hash-sigs/hss_compute.c
    lms-hash-sigs/hss_derive.c
    lms-hash-sigs/hss_generate.c
    lms-hash-sigs/hss_keygen.c
    lms-hash-sigs/hss_param.c
    lms-hash-sigs/hss_reserve.c
    lms-hash-sigs/hss_sign.c
    lms-hash-sigs/hss_sign_inc.c
    lms-hash-sigs/hss_thread_pthread.c
    lms-hash-sigs/hss_verify.c
    lms-hash-sigs/hss_verify_inc.c
    lms-hash-sigs/hss_zeroize.c
    lms-hash-sigs/lm_common.c
    lms-hash-sigs/lm_ots_common.c
    lms-hash-sigs/lm_ots_sign.c
    lms-hash-sigs/lm_ots_verify.c
    lms-hash-sigs/lm_verify.c
    lms-hash-sigs/lms_hash.c
    lms-hash-sigs/lms_keys.c
    lms-hash-sigs/lms_sign.c
    lms-hash-sigs/sha256.c)

add_library(lms-hash-sigs STATIC ${LMS_HASH_SIGS_SOURCES})
target_include_directories(lms-hash-sigs PRIVATE lms-hash-sigs)
target_link_libraries(lms-hash-sigs PRIVATE OpenSSL::Crypto Threads::Threads)

add_executable(bootgen ${BOOTGEN_PARSER_SOURCES} ${BOOTGEN_SOURCES})
target_include_directories(bootgen PRIVATE ${BOOTGEN_INCLUDE_DIRS})
target_link_libraries(bootgen PRIVATE
    lms-hash-sigs
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
    ${CMAKE_DL_LIBS})

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(bootgen PRIVATE -Wall -Wno-reorder -Wno-deprecated-declarations)
    set_source_files_properties(${BOOTGEN_PARSER_SOURCES} PROPERTIES
        COMPILE_OPTIONS "-Wno-deprecated-register")
endif()

if(BOOTGEN_ENABLE_SANITIZERS)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        message(FATAL_ERROR "BOOTGEN_ENABLE_SANITIZERS requires Clang or GCC")
    endif()
    target_compile_options(lms-hash-sigs PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_compile_options(bootgen PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(bootgen PRIVATE -fsanitize=address,undefined)
endif()

include(GNUInstallDirs)
install(TARGETS bootgen RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES LICENSE README.md INSTALL.txt BUILDING.md DESTINATION ${CMAKE_INSTALL_DOCDIR})

set(CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_NAME bootgen)
set(CPACK_PACKAGE_VENDOR "Advanced Micro Devices, Inc.")
set(CPACK_PACKAGE_VERSION "2026.2")
set(CPACK_PACKAGE_FILE_NAME "bootgen-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGING_INSTALL_PREFIX "/")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)

if(BOOTGEN_BUILD_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_test(NAME bootgen.help COMMAND bootgen -help)
        set_tests_properties(bootgen.help PROPERTIES PASS_REGULAR_EXPRESSION "Bootgen v")
        add_test(
            NAME bootgen.zynq-smoke
            COMMAND ${CMAKE_COMMAND}
                -DBOOTGEN=$<TARGET_FILE:bootgen>
                -DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures
                -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/tests/zynq smoke-ü
                -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/zynq_smoke.cmake)
        add_test(
            NAME bootgen.zynqmp-smoke
            COMMAND ${CMAKE_COMMAND}
                -DBOOTGEN=$<TARGET_FILE:bootgen>
                -DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures
                -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/tests/zynqmp-smoke
                -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/zynqmp_smoke.cmake)
        add_test(
            NAME bootgen.zynqmp-auth-smoke
            COMMAND ${CMAKE_COMMAND}
                -DBOOTGEN=$<TARGET_FILE:bootgen>
                -DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures
                -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/tests/zynqmp-auth-smoke
                -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/zynqmp_auth_smoke.cmake)
        add_test(
            NAME bootgen.versal-smoke
            COMMAND ${CMAKE_COMMAND}
                -DBOOTGEN=$<TARGET_FILE:bootgen>
                -DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures
                -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/tests/versal-smoke
                -DARCH=versal
                -DEXPECTED_READ_OUTPUT=partition_type[elf]
                -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/versal_smoke.cmake)
        add_test(
            NAME bootgen.versal-2ve-2vm-smoke
            COMMAND ${CMAKE_COMMAND}
                -DBOOTGEN=$<TARGET_FILE:bootgen>
                -DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures
                -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/tests/versal-2ve-2vm-smoke
                -DARCH=versal_2ve_2vm
                -DEXPECTED_READ_OUTPUT=partition_type[elf]
                -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/versal_smoke.cmake)
        add_test(
            NAME bootgen.spartanup-smoke
            COMMAND ${CMAKE_COMMAND}
                -DBOOTGEN=$<TARGET_FILE:bootgen>
                -DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures
                -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/tests/spartanup-smoke
                -DARCH=spartanup
                -DEXPECTED_READ_OUTPUT=plm_length
                -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/versal_smoke.cmake)
        add_test(
            NAME bootgen.invalid-bif
            COMMAND ${CMAKE_COMMAND}
                -DBOOTGEN=$<TARGET_FILE:bootgen>
                -DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures
                -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/invalid_bif_test.cmake)
    endif()
endif()
