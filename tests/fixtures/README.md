# Zynq smoke fixture

`zynq-smoke.elf` is a tiny, non-secure Cortex-A9 ELF used to exercise Bootgen's
Zynq-7000 ELF import, image construction, and image-read paths. It contains one
four-byte branch-to-self instruction at load and entry address `0x00100000`.
It is not bootable firmware and must never be used on a board.

The ELF is generated from `zynq-smoke.S` and `zynq-smoke.ld` with:

```sh
arm-none-eabi-gcc -nostdlib \
  -Wl,-T,tests/fixtures/zynq-smoke.ld,-e,_start,--build-id=none \
  -o tests/fixtures/zynq-smoke.elf tests/fixtures/zynq-smoke.S
```

The checked-in binary removes a cross-compiler dependency from host-side test
runs. Its ELF header and program header are intentionally small enough to be
inspected in reviews.
