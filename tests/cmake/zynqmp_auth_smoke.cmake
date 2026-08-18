if(NOT DEFINED BOOTGEN OR NOT DEFINED TAMPER_FILE OR NOT DEFINED FIXTURE_DIR OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "BOOTGEN, TAMPER_FILE, FIXTURE_DIR, and OUTPUT_DIR are required")
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
    COMMAND "${BOOTGEN}" -arch zynqmp -image "${TEST_FIXTURE_DIR}/keygen.bif" -generate_keys pem
    WORKING_DIRECTORY "${TEST_FIXTURE_DIR}"
    RESULT_VARIABLE KEYGEN_RESULT
    OUTPUT_VARIABLE KEYGEN_OUTPUT
    ERROR_VARIABLE KEYGEN_ERROR)
if(NOT KEYGEN_RESULT EQUAL 0 OR NOT EXISTS "${PRIMARY_KEY}" OR NOT EXISTS "${SECONDARY_KEY}")
    cleanup_test_keys()
    message(FATAL_ERROR "RSA key generation failed (${KEYGEN_RESULT}):\n${KEYGEN_OUTPUT}\n${KEYGEN_ERROR}")
endif()
file(CHMOD "${PRIMARY_KEY}" "${SECONDARY_KEY}" PERMISSIONS OWNER_READ OWNER_WRITE)

file(WRITE "${TEST_FIXTURE_DIR}/authenticated.bif" "all:\n{\n    [pskfile] primary.pem\n    [sskfile] secondary.pem\n    [bootloader, destination_cpu=a53-0, authentication=rsa] zynq-smoke.elf\n}\n")
set(BOOT_IMAGE "${OUTPUT_DIR}/BOOT.BIN")
execute_process(
    COMMAND "${BOOTGEN}" -arch zynqmp -image "${TEST_FIXTURE_DIR}/authenticated.bif" -o "${BOOT_IMAGE}" -w on
    WORKING_DIRECTORY "${TEST_FIXTURE_DIR}"
    RESULT_VARIABLE CREATE_RESULT
    OUTPUT_VARIABLE CREATE_OUTPUT
    ERROR_VARIABLE CREATE_ERROR)
if(NOT CREATE_RESULT EQUAL 0)
    cleanup_test_keys()
    message(FATAL_ERROR "Authenticated ZynqMP smoke image creation failed (${CREATE_RESULT}):\n${CREATE_OUTPUT}\n${CREATE_ERROR}")
endif()

execute_process(
    COMMAND "${BOOTGEN}" -arch zynqmp -verify "${BOOT_IMAGE}"
    RESULT_VARIABLE VERIFY_RESULT
    OUTPUT_VARIABLE VERIFY_OUTPUT
    ERROR_VARIABLE VERIFY_ERROR)
if(NOT VERIFY_RESULT EQUAL 0)
    cleanup_test_keys()
    message(FATAL_ERROR "Authenticated ZynqMP smoke image verification failed (${VERIFY_RESULT}):\n${VERIFY_OUTPUT}\n${VERIFY_ERROR}")
endif()

set(TAMPERED_BOOT_IMAGE "${OUTPUT_DIR}/BOOT-tampered.BIN")
execute_process(
    COMMAND "${TAMPER_FILE}" "${BOOT_IMAGE}" "${TAMPERED_BOOT_IMAGE}"
    RESULT_VARIABLE TAMPER_RESULT
    OUTPUT_VARIABLE TAMPER_OUTPUT
    ERROR_VARIABLE TAMPER_ERROR)
if(NOT TAMPER_RESULT EQUAL 0)
    cleanup_test_keys()
    message(FATAL_ERROR "Unable to modify authenticated ZynqMP smoke image (${TAMPER_RESULT}):\n${TAMPER_OUTPUT}\n${TAMPER_ERROR}")
endif()
execute_process(
    COMMAND "${BOOTGEN}" -arch zynqmp -verify "${TAMPERED_BOOT_IMAGE}"
    RESULT_VARIABLE TAMPER_VERIFY_RESULT
    OUTPUT_VARIABLE TAMPER_VERIFY_OUTPUT
    ERROR_VARIABLE TAMPER_VERIFY_ERROR)
if(TAMPER_VERIFY_RESULT EQUAL 0)
    cleanup_test_keys()
    message(FATAL_ERROR "Modified authenticated ZynqMP image unexpectedly verified:\n${TAMPER_VERIFY_OUTPUT}\n${TAMPER_VERIFY_ERROR}")
endif()

cleanup_test_keys()
