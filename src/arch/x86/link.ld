ENTRY(_start)

SECTIONS
{
  . = 1M; /* We start at the 1MB spot */

  .text BLOCK(4K) : ALIGN(4K)
  {
    *(.text)
  }

  .rodata BLOCK(4K) : ALIGN(4K)
  {
    *(.rodata)
  }

  .data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

  .bss BLOCK(4K) : ALIGN(4K)
  {
    *(.bss)
  }
}
