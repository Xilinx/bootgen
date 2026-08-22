if(NOT DEFINED BOOTGEN OR NOT DEFINED FIXTURE_DIR OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "BOOTGEN, FIXTURE_DIR, and OUTPUT_DIR are required")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIR}")
set(TEST_FIXTURE_DIR "${OUTPUT_DIR}/fixture")
file(MAKE_DIRECTORY "${TEST_FIXTURE_DIR}")
file(CHMOD "${TEST_FIXTURE_DIR}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
file(COPY "${FIXTURE_DIR}/zynq-smoke.elf" DESTINATION "${TEST_FIXTURE_DIR}")

set(PRIMARY_KEY "${TEST_FIXTURE_DIR}/primary.pem")
set(SECONDARY_KEY "${TEST_FIXTURE_DIR}/secondary.pem")
function(cleanup_test_keys)
    file(REMOVE "${PRIMARY_KEY}" "${SECONDARY_KEY}")
endfunction()

file(WRITE "${TEST_FIXTURE_DIR}/keygen.bif" "all:\n{\n    [pskfile] primary.pem\n    [sskfile] secondary.pem\n}\n")
execute_process(
    COMMAND "${BOOTGEN}" -arch versal_2ve_2vm -image "${TEST_FIXTURE_DIR}/keygen.bif" -generate_keys ecdsa
    WORKING_DIRECTORY "${TEST_FIXTURE_DIR}"
    RESULT_VARIABLE KEYGEN_RESULT
    OUTPUT_VARIABLE KEYGEN_OUTPUT
    ERROR_VARIABLE KEYGEN_ERROR)
if(NOT KEYGEN_RESULT EQUAL 0 OR NOT EXISTS "${PRIMARY_KEY}" OR NOT EXISTS "${SECONDARY_KEY}")
    cleanup_test_keys()
    message(FATAL_ERROR "Versal 2VE/2VM ECDSA key generation failed (${KEYGEN_RESULT}):\n${KEYGEN_OUTPUT}\n${KEYGEN_ERROR}")
endif()
file(CHMOD "${PRIMARY_KEY}" "${SECONDARY_KEY}" PERMISSIONS OWNER_READ OWNER_WRITE)

file(WRITE "${TEST_FIXTURE_DIR}/authenticated.bif" "all:\n{\n    pskfile = primary.pem\n    sskfile = secondary.pem\n    image\n    {\n        name = image1, id = 0x1c000001\n        { type = bootloader, authentication = ecdsa, file = zynq-smoke.elf }\n    }\n}\n")
set(BOOT_IMAGE "${OUTPUT_DIR}/BOOT.BIN")
execute_process(
    COMMAND "${BOOTGEN}" -arch versal_2ve_2vm -image "${TEST_FIXTURE_DIR}/authenticated.bif" -o "${BOOT_IMAGE}" -w on
    WORKING_DIRECTORY "${TEST_FIXTURE_DIR}"
    RESULT_VARIABLE CREATE_RESULT
    OUTPUT_VARIABLE CREATE_OUTPUT
    ERROR_VARIABLE CREATE_ERROR)
if(NOT CREATE_RESULT EQUAL 0)
    cleanup_test_keys()
    message(FATAL_ERROR "Authenticated Versal 2VE/2VM image creation failed (${CREATE_RESULT}):\n${CREATE_OUTPUT}\n${CREATE_ERROR}")
endif()
file(SIZE "${BOOT_IMAGE}" BOOT_IMAGE_SIZE)
if(BOOT_IMAGE_SIZE LESS 2048)
    cleanup_test_keys()
    message(FATAL_ERROR "Authenticated Versal 2VE/2VM image is unexpectedly small: ${BOOT_IMAGE_SIZE} bytes")
endif()

cleanup_test_keys()
