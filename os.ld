OUTPUT_ARCH( "riscv" )

ENTRY( _start )

/*
  # https://sourceware.org/binutils/docs/ld/MEMORY.html
	The linker's default configuration permits allocation of all available memory, we can override this by using the MEMORY command.
	You can use it to describe: 
	1. Which memory regions may be used by the linker.
	2. Which memory regions it must avoid.
	3. Assign sections to particular memory regions.
  MEMORY
  {
      name [(attr)] : ORIGIN = origin, LENGTH = len
      ......
  }
	(attr):
	1. R --- read-only section
	2. W --- read/write section
	3. X --- executable section
	4. A --- allocatable section
	5. I/L - initialized section
	6. ! --- invert the sense of any of the attributes that follow
 */
MEMORY
{
	ram   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
}

/*
  #  https://sourceware.org/binutils/docs/ld/SECTIONS.html
  '.' = current memory location
 */
SECTIONS
{
	.text : {
		PROVIDE(_text_start = .);
		*(.text .text.*)
		PROVIDE(_text_end = .);
	} >ram

	.rodata : {
		PROVIDE(_rodata_start = .);
		*(.rodata .rodata.*)
		*(.srodata .srodata.*)
		PROVIDE(_rodata_end = .);
	} >ram

	.data : {
		/*
		   . = ALIGN(4096) tells the linker to align the current memory
		   location to 4096 bytes. This will insert padding bytes until
		   current location becomes aligned on 4096-byte boundary.
		   This is because our paging system's resolution is 4,096 bytes.
		 */
		. = ALIGN(4096);
		PROVIDE(_data_start = .);
		/*
		   sdata and data are essentially the same thing. We do not need
		   to distinguish sdata from data.
		 */
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);
	} >ram


	.bss :{
		/*
		 # https://sourceware.org/binutils/docs/ld/Input-Section-Common.html
		 */
		PROVIDE(_bss_start = .);
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		*(COMMON)
		PROVIDE(_bss_end = .);
	} >ram


	.task1 :{
		PROVIDE(_task1_start = .);
		*(.task1)
		PROVIDE(_task1_end = .);
	} >ram

	.task2 :{
		PROVIDE(_task2_start = .);
		*(.task2)
		PROVIDE(_task2_end = .);
	} >ram

	.task3 :{
		PROVIDE(_task3_start = .);
		*(.task3)
		PROVIDE(_task3_end = .);
	} >ram

/*	.[taskName] :{
		PROVIDE(_[taskName]_start = .);
		*(.[taskName])
		PROVIDE(_[taskName]_end = .);
	} >ram
*/

	PROVIDE(_memory_start = ORIGIN(ram));
	PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));

	PROVIDE(_heap_start = _bss_end);
	PROVIDE(_heap_size = _memory_end - _heap_start);
}
