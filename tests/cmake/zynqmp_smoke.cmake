if(NOT DEFINED BOOTGEN OR NOT DEFINED FIXTURE_DIR OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "BOOTGEN, FIXTURE_DIR, and OUTPUT_DIR are required")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIR}")
set(TEST_FIXTURE_DIR "${OUTPUT_DIR}/fixture")
file(MAKE_DIRECTORY "${TEST_FIXTURE_DIR}")
file(COPY "${FIXTURE_DIR}/zynqmp-smoke.bif" "${FIXTURE_DIR}/zynq-smoke.elf" DESTINATION "${TEST_FIXTURE_DIR}")
set(BOOT_IMAGE "${OUTPUT_DIR}/BOOT.BIN")

execute_process(
    COMMAND "${BOOTGEN}" -arch zynqmp -image "${TEST_FIXTURE_DIR}/zynqmp-smoke.bif" -o "${BOOT_IMAGE}" -w on
    WORKING_DIRECTORY "${TEST_FIXTURE_DIR}"
    RESULT_VARIABLE CREATE_RESULT
    OUTPUT_VARIABLE CREATE_OUTPUT
    ERROR_VARIABLE CREATE_ERROR)
if(NOT CREATE_RESULT EQUAL 0)
    message(FATAL_ERROR "ZynqMP smoke image creation failed (${CREATE_RESULT}):\n${CREATE_OUTPUT}\n${CREATE_ERROR}")
endif()

set(REPEAT_BOOT_IMAGE "${OUTPUT_DIR}/BOOT-repeat.BIN")
execute_process(
    COMMAND "${BOOTGEN}" -arch zynqmp -image "${TEST_FIXTURE_DIR}/zynqmp-smoke.bif" -o "${REPEAT_BOOT_IMAGE}" -w on
    WORKING_DIRECTORY "${TEST_FIXTURE_DIR}"
    RESULT_VARIABLE REPEAT_RESULT
    OUTPUT_VARIABLE REPEAT_OUTPUT
    ERROR_VARIABLE REPEAT_ERROR)
if(NOT REPEAT_RESULT EQUAL 0)
    message(FATAL_ERROR "Repeated ZynqMP smoke image creation failed (${REPEAT_RESULT}):\n${REPEAT_OUTPUT}\n${REPEAT_ERROR}")
endif()
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E compare_files "${BOOT_IMAGE}" "${REPEAT_BOOT_IMAGE}"
    RESULT_VARIABLE COMPARE_RESULT)
if(NOT COMPARE_RESULT EQUAL 0)
    message(FATAL_ERROR "Repeated ZynqMP smoke images are not byte-identical")
endif()

file(SIZE "${BOOT_IMAGE}" BOOT_IMAGE_SIZE)
if(BOOT_IMAGE_SIZE LESS 2048)
    message(FATAL_ERROR "ZynqMP smoke image is unexpectedly small: ${BOOT_IMAGE_SIZE} bytes")
endif()

execute_process(
    COMMAND "${BOOTGEN}" -arch zynqmp -read "${BOOT_IMAGE}"
    RESULT_VARIABLE READ_RESULT
    OUTPUT_VARIABLE READ_OUTPUT
    ERROR_VARIABLE READ_ERROR)
if(NOT READ_RESULT EQUAL 0)
    message(FATAL_ERROR "ZynqMP smoke image inspection failed (${READ_RESULT}):\n${READ_OUTPUT}\n${READ_ERROR}")
endif()

string(FIND "${READ_OUTPUT}" "zynq-smoke.elf" IMAGE_NAME_OFFSET)
string(FIND "${READ_OUTPUT}" "exec_addr_lo (0x10) : 0x00100000" EXEC_ADDRESS_OFFSET)
string(FIND "${READ_OUTPUT}" "core [a53-0]" CORE_OFFSET)
if(IMAGE_NAME_OFFSET EQUAL -1 OR EXEC_ADDRESS_OFFSET EQUAL -1 OR CORE_OFFSET EQUAL -1)
    message(FATAL_ERROR "ZynqMP smoke image metadata does not match the input ELF:\n${READ_OUTPUT}")
endif()
