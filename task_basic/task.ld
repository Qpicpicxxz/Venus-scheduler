OUTPUT_ARCH( "riscv" )

ENTRY( _start )

# venus/src/rtl/venus/scheduler/scheduler_core_wrapper.sv
# [PPT.65] Memory Map(Bi-directional AXI4) - Venus Block
/*
 * 1. R --- read-only section
 * 2. W --- read/write section
 * 3. X --- executable section
 * 4. A --- allocatable section
 * 5. I/L - initialized section
 */
MEMORY
{
	ispm   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 1M
}

SECTIONS
{
	.text : {
		PROVIDE(_text_start = .);
		*(.text .text.*)
		PROVIDE(_text_end = .);
	} >ispm

	.rodata : {
		PROVIDE(_rodata_start = .);
		*(.rodata .rodata.*)
		*(.srodata .srodata.*)
		PROVIDE(_rodata_end = .);
	} >ispm

	.data : {
		PROVIDE(_data_start = .);
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);
	} >ispm

	.bss :{
		PROVIDE(_bss_start = .);
		*(.bss.stack)
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		*(COMMON)
		PROVIDE(_bss_end = .);
	} >ispm

	PROVIDE(_memory_start = ORIGIN(ispm));
	PROVIDE(_memory_end = ORIGIN(ispm) + LENGTH(ispm));

	PROVIDE(_heap_start = _bss_end);
	PROVIDE(_heap_size = _memory_end - _heap_start);
}


