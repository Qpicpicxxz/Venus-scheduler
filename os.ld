OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
	ram   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
}

start = 0;


SECTIONS
{
	.text : {
		. = 0x00000000;
		*(.startup)
		KEEP (*(.startup))
		. = 0x00000020;
		*(.startirq)
		KEEP (*(.startirq))
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
		. = ALIGN(4096);
		PROVIDE(_data_start = .);
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);
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

	.bss :{
		PROVIDE(_bss_start = .);
		*(.bss.stack)
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		*(COMMON)
		PROVIDE(_bss_end = .);
	} >ram

	PROVIDE(_memory_start = ORIGIN(ram));
	PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));

	PROVIDE(_heap_start = _bss_end);
	PROVIDE(_heap_size = _memory_end - _heap_start);
}
