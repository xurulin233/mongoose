ENTRY(Reset_Handler);
MEMORY {
  flash(rx) : ORIGIN = 0x00000000, LENGTH = 1024k
  sram(rwx) : ORIGIN = 0x20000000, LENGTH = 256k 
}
_estack = ORIGIN(sram) + LENGTH(sram);    /* End of RAM. stack points here */

SECTIONS {
  .vectors  : { KEEP(*(.isr_vector)) }  > flash
  .text     : { *(.text* .text.*) }     > flash
  .rodata   : { *(.rodata*) }           > flash

  .data : {
    _sdata = .;
    *(.first_data)
    *(.ram)
    *(.data SORT(.data.*))
    _edata = .;
  } > sram AT > flash
  _sidata = LOADADDR(.data);

  .bss : {
    _sbss = .;
    *(.bss SORT(.bss.*) COMMON)
    _ebss = .;
  } > sram

  . = ALIGN(8);
  _end = .;
}
