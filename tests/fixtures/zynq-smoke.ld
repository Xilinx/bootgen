ENTRY(_start)

SECTIONS
{
    . = 0x00100000;
    .text : { *(.text*) }
}
