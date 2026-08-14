if(NOT DEFINED BOOTGEN OR NOT DEFINED FIXTURE_DIR OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "BOOTGEN, FIXTURE_DIR, and OUTPUT_DIR are required")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIR}")
set(TEST_FIXTURE_DIR "${OUTPUT_DIR}/fixture")
file(MAKE_DIRECTORY "${TEST_FIXTURE_DIR}")
set(TEST_ELF "${TEST_FIXTURE_DIR}/zynq smoke-ü.elf")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E copy "${FIXTURE_DIR}/zynq-smoke.elf" "${TEST_ELF}"
    RESULT_VARIABLE COPY_RESULT)
if(NOT COPY_RESULT EQUAL 0)
    message(FATAL_ERROR "Unable to create the Zynq smoke ELF fixture")
endif()
file(WRITE "${TEST_FIXTURE_DIR}/zynq-smoke.bif" "the_ROM_image:\n{\n    [bootloader] \"zynq smoke-ü.elf\"\n}\n")
set(BOOT_IMAGE "${OUTPUT_DIR}/BOOT.BIN")

execute_process(
    COMMAND "${BOOTGEN}" -arch zynq -image "${TEST_FIXTURE_DIR}/zynq-smoke.bif" -o "${BOOT_IMAGE}" -w on
    WORKING_DIRECTORY "${TEST_FIXTURE_DIR}"
    RESULT_VARIABLE CREATE_RESULT
    OUTPUT_VARIABLE CREATE_OUTPUT
    ERROR_VARIABLE CREATE_ERROR)
if(NOT CREATE_RESULT EQUAL 0)
    message(FATAL_ERROR "Zynq smoke image creation failed (${CREATE_RESULT}):\n${CREATE_OUTPUT}\n${CREATE_ERROR}")
endif()

file(SIZE "${BOOT_IMAGE}" BOOT_IMAGE_SIZE)
if(BOOT_IMAGE_SIZE LESS 2048)
    message(FATAL_ERROR "Zynq smoke image is unexpectedly small: ${BOOT_IMAGE_SIZE} bytes")
endif()

execute_process(
    COMMAND "${BOOTGEN}" -arch zynq -read "${BOOT_IMAGE}"
    RESULT_VARIABLE READ_RESULT
    OUTPUT_VARIABLE READ_OUTPUT
    ERROR_VARIABLE READ_ERROR)
if(NOT READ_RESULT EQUAL 0)
    message(FATAL_ERROR "Zynq smoke image inspection failed (${READ_RESULT}):\n${READ_OUTPUT}\n${READ_ERROR}")
endif()

string(FIND "${READ_OUTPUT}" "zynq smoke-ü.elf" IMAGE_NAME_OFFSET)
string(FIND "${READ_OUTPUT}" "fsbl_load_address (0x38) : 0x00100000" LOAD_ADDRESS_OFFSET)
string(FIND "${READ_OUTPUT}" "fsbl_exec_address (0x3C) : 0x00100000" EXEC_ADDRESS_OFFSET)
if(IMAGE_NAME_OFFSET EQUAL -1 OR LOAD_ADDRESS_OFFSET EQUAL -1 OR EXEC_ADDRESS_OFFSET EQUAL -1)
    message(FATAL_ERROR "Zynq smoke image metadata does not match the input ELF:\n${READ_OUTPUT}")
endif()
