if(NOT DEFINED BOOTGEN OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "BOOTGEN and OUTPUT_DIR are required")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIR}")
set(MALFORMED_IMAGE "${OUTPUT_DIR}/truncated-boot-image.bin")
# A boot image contains headers larger than this input.  Keeping the fixture
# intentionally short exercises bounds checks before any image data is used.
file(WRITE "${MALFORMED_IMAGE}" "BOOTGEN")

foreach(ARCHITECTURE zynq zynqmp versal versal_2ve_2vm spartanup)
    execute_process(
        COMMAND "${BOOTGEN}" -arch "${ARCHITECTURE}" -read "${MALFORMED_IMAGE}"
        RESULT_VARIABLE READ_RESULT
        OUTPUT_VARIABLE READ_OUTPUT
        ERROR_VARIABLE READ_ERROR)
    if(READ_RESULT EQUAL 0)
        message(FATAL_ERROR "${ARCHITECTURE} accepted a truncated boot image:\n${READ_OUTPUT}\n${READ_ERROR}")
    endif()
endforeach()
