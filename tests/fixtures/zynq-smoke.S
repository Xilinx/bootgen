/* Minimal Cortex-A9 image used only for host-side Bootgen smoke testing. */
.syntax unified
.cpu cortex-a9

.section .text
.global _start
.type _start, %function
_start:
    b _start
