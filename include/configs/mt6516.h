#ifndef __CONFIG_H
#define __CONFIG_H


/*
 * High Level Configuration Options
 * (easy to change)
 */


#define DEBUG

#undef CONFIG_USE_IRQ			/* we don't need IRQ/FIQ stuff	*/


#define CONFIG_SYS_MAX_NAND_DEVICE  1
#define CONFIG_SYS_NAND_BASE        0x80000000
#define CONFIG_SYS_NAND_MAX_CHIPS 1
/* U-boot Load Address */

#define CONFIG_SYS_TEXT_BASE         0x40020000
#define CONFIG_SYS_INIT_SP_ADDR      0x40010000

/* #define CONFIG_SYS_TEXT_BASE         0x00001000  */

/*
 * Miscellaneous configurable options
 */


#define CONFIG_SYS_CBSIZE	256			/* Console I/O Buffer Size*/
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	16			/* max number of command args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE		/* Boot Argument Buffer Size*/


/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN                   (1024*1024)




#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }


#define CONFIG_SYS_LOAD_ADDR		0x00000000


#define CONFIG_SYS_ICACHE_OFF
#define CONFIG_SYS_DCACHE_OFF

/*-----------------------------------------------------------------------
 * Physical Memory Map
*/ 
/* #define CONFIG_SYS_DRAM_TEST */

#define CONFIG_SYS_SDRAM_BASE 0x00000000
#define CONFIG_NR_DRAM_BANKS	1	   /*  we have 1 bank of DRAM */

#define PHYS_SDRAM_1_SIZE	0x8000000 /* 128 MB */

/* additions for new relocation code */


#define CONFIG_ENV_SIZE		0x20000     /* Total Size of Environment */

#endif	/* __CONFIG_H */
