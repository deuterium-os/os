ENTRY(_start)

mmio        = 0xfffffffff8000000;
fb          = 0xfffffffffc000000;   /* Linear frame buffer mapped */
bootboot    = 0xffffffffffe00000;   /* BOOTBOOT structure */
environment = 0xffffffffffe01000;

PHDRS
{
  boot PT_LOAD;
}
SECTIONS
{
    . = 0xffffffffffe02000;
    .text : {
        KEEP(*(.text.boot)) *(.text .text.*)   /* code */
        *(.rodata .rodata.*)                   /* data */
        *(.data .data.*)
    } :boot
    .bss (NOLOAD) : {                          /* bss */
        . = ALIGN(16);
        *(.bss .bss.*)
        *(COMMON)
    } :boot

    /DISCARD/ : { *(.comment) }
}

