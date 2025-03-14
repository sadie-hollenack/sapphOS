ENTRY(start)

SECTIONS {
    /* starts at 0x10000000 to not overlap with the kernels address space */
    . = 0x1000000;

    /* machine code */
    .text :{
        KEEP(*(.text.start));
        *(.text .text.*);
    }

    /* read only data */
    .rodata : ALIGN(4) {
        *(.data .data.*);
    }

    .bss : ALIGN(4) {
        *(.bss .bss.* .sbss .sbss.*);

        . = ALIGN(16);
        . += 64 * 1024;
        __stack_top = .;

        ASSERT(. < 0x18000000, "too large executable");
    }
}