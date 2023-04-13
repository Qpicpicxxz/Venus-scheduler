OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
	ram   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
	.text : {
		. = 0x00000000;
		*(.startup)
		KEEP (*(.startup))
		. = 0x00000020;
		*(.startirq)
		KEEP (*(.startirq))
		*(.text .text.*)
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
	
# python end jacklight

	.bss :{
		PROVIDE(_bss_start = .);
		*(.bss.stack)
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		*(COMMON)
		PROVIDE(_bss_end = .);
	} >ram
}
