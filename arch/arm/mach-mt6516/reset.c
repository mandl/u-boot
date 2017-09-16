/*
 * (C) Copyright 2013
 *
 * GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/mt6516.h>


/* Reset the cpu by telling the reset controller to do so */
void reset_cpu(ulong ignored)
{
	// TODO reset
	/* never reached */
	while (1)
		;
}
