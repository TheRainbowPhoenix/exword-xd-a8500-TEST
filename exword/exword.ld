/* EX-word module memory layout adapted from the devkitSH4 script:
 * MaxSignal/buildscripts dksh4/crtls/exword.ld
 * https://github.com/MaxSignal/buildscripts/blob/master/dksh4/crtls/exword.ld
 *
 * Do not substitute the ClassPad linker script here. The EX-word loader
 * expects its own ROM/RAM addresses and entry point.
 */
ENTRY("_start")
MEMORY
{
  rom (rx) : ORIGIN = 0x70000080, LENGTH = 0x1FF80
  ram (w)  : ORIGIN = 0x7400004c, LENGTH = 0x7FB4
}
SECTIONS
{
  .text :
  {
    _stext = .;
    *(.text)
    *(.text*)
    *(.rodata)
    *(.rodata*)
    _etext = .;
  } > rom
  .bss :
  {
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > ram
  .data :
  {
    _sdata = .;
    *(.data)
    *(.data*)
    _edata = .;
  } > ram AT > rom
  /DISCARD/ :
  {
    *(.comment)
    *(.eh_frame)
  }
}
