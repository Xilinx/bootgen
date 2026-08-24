if(NOT DEFINED BOOTGEN OR NOT DEFINED FIXTURE_DIR OR NOT DEFINED OUTPUT_DIR OR NOT DEFINED ARCH)
    message(FATAL_ERROR "BOOTGEN, FIXTURE_DIR, OUTPUT_DIR, and ARCH are required")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIR}")
set(TEST_FIXTURE_DIR "${OUTPUT_DIR}/fixture")
file(MAKE_DIRECTORY "${TEST_FIXTURE_DIR}")
file(COPY "${FIXTURE_DIR}/zynq-smoke.elf" DESTINATION "${TEST_FIXTURE_DIR}")
file(WRITE "${TEST_FIXTURE_DIR}/minimal.cdo" "version 2.0\nwrite 0xf1260050 0\n")
file(WRITE "${TEST_FIXTURE_DIR}/versal-cdo.bif" "all:\n{\n    image\n    {\n        name = image1, id = 0x1c000001\n        { type = bootloader, encryption = aes, keysrc = bbram_red_key, aeskeyfile = boot.nky, file = zynq-smoke.elf }\n        { type = cdo, encryption = aes, keysrc = bbram_red_key, aeskeyfile = cdo.nky, file = minimal.cdo }\n    }\n}\n")

set(BOOT_IMAGE "${OUTPUT_DIR}/BOOT.BIN")
execute_process(
    COMMAND "${BOOTGEN}" -arch "${ARCH}" -image "${TEST_FIXTURE_DIR}/versal-cdo.bif" -o "${BOOT_IMAGE}" -w on -p xcvc1902
    WORKING_DIRECTORY "${TEST_FIXTURE_DIR}"
    RESULT_VARIABLE CREATE_RESULT
    OUTPUT_VARIABLE CREATE_OUTPUT
    ERROR_VARIABLE CREATE_ERROR)
if(NOT CREATE_RESULT EQUAL 0)
    message(FATAL_ERROR "${ARCH} CDO image creation failed (${CREATE_RESULT}):\n${CREATE_OUTPUT}\n${CREATE_ERROR}")
endif()

file(SIZE "${BOOT_IMAGE}" BOOT_IMAGE_SIZE)
if(BOOT_IMAGE_SIZE LESS 512)
    message(FATAL_ERROR "${ARCH} CDO smoke image is unexpectedly small: ${BOOT_IMAGE_SIZE} bytes")
endif()

if(DEFINED EXPECT_READ_CDO_PARTITION AND EXPECT_READ_CDO_PARTITION)
    execute_process(
        COMMAND "${BOOTGEN}" -arch "${ARCH}" -read "${BOOT_IMAGE}"
        RESULT_VARIABLE READ_RESULT
        OUTPUT_VARIABLE READ_OUTPUT
        ERROR_VARIABLE READ_ERROR)
    if(NOT READ_RESULT EQUAL 0)
        message(FATAL_ERROR "${ARCH} CDO image inspection failed (${READ_RESULT}):\n${READ_OUTPUT}\n${READ_ERROR}")
    endif()

    string(FIND "${READ_OUTPUT}" "partition_type[cdo]" CDO_OFFSET)
    if(CDO_OFFSET EQUAL -1)
        message(FATAL_ERROR "${ARCH} CDO smoke image does not contain a CDO partition:\n${READ_OUTPUT}")
    endif()
endif()

if(DEFINED EXPECT_LEGACY_FAILURE AND EXPECT_LEGACY_FAILURE)
    file(WRITE "${TEST_FIXTURE_DIR}/legacy.cdo" "version 1.0\nsection 0\nwrite 0xf1260050 0\n")
    file(WRITE "${TEST_FIXTURE_DIR}/versal-legacy-cdo.bif" "all:\n{\n    image\n    {\n        name = image1, id = 0x1c000001\n        { type = bootloader, file = zynq-smoke.elf }\n        { type = cdo, file = legacy.cdo }\n    }\n}\n")
    execute_process(
        COMMAND "${BOOTGEN}" -arch "${ARCH}" -image "${TEST_FIXTURE_DIR}/versal-legacy-cdo.bif" -o "${OUTPUT_DIR}/legacy-BOOT.BIN" -w on
        WORKING_DIRECTORY "${TEST_FIXTURE_DIR}"
        RESULT_VARIABLE LEGACY_RESULT
        OUTPUT_VARIABLE LEGACY_OUTPUT
        ERROR_VARIABLE LEGACY_ERROR)
    if(LEGACY_RESULT EQUAL 0 OR NOT LEGACY_RESULT MATCHES "^[1-9][0-9]*$")
        message(FATAL_ERROR "${ARCH} accepted or crashed on an unsupported legacy CDO (result: ${LEGACY_RESULT}):\n${LEGACY_OUTPUT}\n${LEGACY_ERROR}")
    endif()
endif()
