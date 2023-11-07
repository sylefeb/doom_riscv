MEMORY
{
    ROM     (xr)  : ORIGIN = 0x00000000, LENGTH = 0x100000	/* 1BM */
    PSRAM   (xrw) : ORIGIN = 0x00100000, LENGTH = 0x700000	/* 7MB */
}
ENTRY(_start)
SECTIONS {
    __stacktop = ORIGIN(PSRAM) + LENGTH(PSRAM);
    .text :
    {
        . = ALIGN(4);
        *(.text.start)
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.srodata)
        *(.srodata*)
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } >ROM
    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = .;
        _ram_start = .;
        . = ALIGN(4);
        *(.data)
        *(.data*)
        *(.sdata)
        *(.sdata*)
        . = ALIGN(4);
        _edata = .;
    } >PSRAM
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(.sbss)
        *(.sbss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >PSRAM
    .heap :
    {
        . = ALIGN(4);
        _heap_start = .;
        . = ALIGN(4);
    } >PSRAM
}
