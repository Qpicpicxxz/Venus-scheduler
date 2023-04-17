OUTPUT_ARCH( "riscv" )

ENTRY( _start )

# venus/src/rtl/venus/scheduler/scheduler_core_wrapper.sv
# [PPT.65] Memory Map(Bi-directional AXI4) - Venus Block
MEMORY
{
	ispm   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 2M
  dspm   (wxa!ri) : ORIGIN = 0x80200000, LENGTH = 1M
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
		. = ALIGN(4096);
		PROVIDE(_data_start = .);
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);
	} >ispm

# python end jacklight

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

