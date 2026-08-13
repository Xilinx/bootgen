if(NOT DEFINED BOOTGEN OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "BOOTGEN and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${BOOTGEN}" -arch zynq -image invalid.bif
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE RESULT
    OUTPUT_VARIABLE OUTPUT
    ERROR_VARIABLE ERROR)
if(RESULT EQUAL 0)
    message(FATAL_ERROR "Malformed BIF unexpectedly succeeded:\n${OUTPUT}\n${ERROR}")
endif()

string(FIND "${OUTPUT}${ERROR}" "syntax error" ERROR_OFFSET)
if(ERROR_OFFSET EQUAL -1)
    message(FATAL_ERROR "Malformed BIF did not report a syntax error:\n${OUTPUT}\n${ERROR}")
endif()
