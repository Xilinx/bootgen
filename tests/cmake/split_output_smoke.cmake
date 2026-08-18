if(NOT DEFINED BOOTGEN OR NOT DEFINED FIXTURE_DIR OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "BOOTGEN, FIXTURE_DIR, and OUTPUT_DIR are required")
endif()

set(WORK_DIR "${OUTPUT_DIR}/work")
set(INPUT_DIR "${WORK_DIR}/input")
file(MAKE_DIRECTORY "${INPUT_DIR}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E copy "${FIXTURE_DIR}/zynq-smoke.elf" "${WORK_DIR}/zynq-smoke.elf"
    RESULT_VARIABLE COPY_RESULT)
if(NOT COPY_RESULT EQUAL 0)
    message(FATAL_ERROR "Unable to create the split-output ELF fixture")
endif()
file(WRITE "${INPUT_DIR}/zynq-split.bif" "the_ROM_image:\n{\n    [bootloader] zynq-smoke.elf\n}\n")

execute_process(
    COMMAND "${BOOTGEN}" -arch zynq -image input/zynq-split.bif -split bin -w on
    WORKING_DIRECTORY "${WORK_DIR}"
    RESULT_VARIABLE CREATE_RESULT
    OUTPUT_VARIABLE CREATE_OUTPUT
    ERROR_VARIABLE CREATE_ERROR)
if(NOT CREATE_RESULT EQUAL 0)
    message(FATAL_ERROR "Split output creation failed (${CREATE_RESULT}):\n${CREATE_OUTPUT}\n${CREATE_ERROR}")
endif()

set(EXPECTED_OUTPUT "${WORK_DIR}/zynq-split.bin")
set(UNEXPECTED_OUTPUT "${INPUT_DIR}/zynq-split.bin")
if(NOT EXISTS "${EXPECTED_OUTPUT}")
    message(FATAL_ERROR "Split output was not created in the working directory")
endif()
if(EXISTS "${UNEXPECTED_OUTPUT}")
    message(FATAL_ERROR "Split output incorrectly retained the BIF input directory")
endif()
file(SIZE "${EXPECTED_OUTPUT}" OUTPUT_SIZE)
if(OUTPUT_SIZE LESS 2048)
    message(FATAL_ERROR "Split output is unexpectedly small: ${OUTPUT_SIZE} bytes")
endif()
