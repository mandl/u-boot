/*
 * (C) Copyright 2013
 *
 * GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/mt6516.h>



#if defined(CONFIG_DISPLAY_CPUINFO)
int print_cpuinfo(void)
{
	printf("CPU:              0x%x\n",readw(HW_CODE));
	printf("Hardware Version: 0x%x\n",readw(HW_VER));
	printf("Software Version: 0x%x\n",readw(SW_VER));

	return 0;
}
#endif
