/*
 * Copyright (C) 2017
 *
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/processor.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

#include <asm/arch/mt6516.h>

#define MPLL_CON                 (PLL_BASE+0x0000)
#define APLL_CON0                (PLL_BASE+0x0018)
#define APLL_CON1                (PLL_BASE+0x001C)
#define LPLL_CON0                (PLL_BASE+0x0020)
#define LPLL_CON1                (PLL_BASE+0x0024)

#define PDN_CON                  (PLL_BASE+0x0010)
#define CLK_CON                  (PLL_BASE+0x0014)
#define MPLL                     (PLL_BASE+0x0020)
#define MPLL2                    (PLL_BASE+0x0024)
#define UPLL                     (PLL_BASE+0x0030)
#define UPLL2                    (PLL_BASE+0x0034)
#define CPLL                     (PLL_BASE+0x0038)
#define CPLL2                    (PLL_BASE+0x003C)
#define TPLL                     (PLL_BASE+0x0040)
#define TPLL2                    (PLL_BASE+0x0044)
#define CPLL3                    (PLL_BASE+0x0048)
#define PLL_RES_CON0             (PLL_BASE+0x004c)
#define PLL_BIAS                 (PLL_BASE+0x0050)
#define MCPLL                    (PLL_BASE+0x0058)
#define MCPLL2                   (PLL_BASE+0x005C)
#define CEVAPLL                  (PLL_BASE+0x0060)
#define CEVAPLL2                 (PLL_BASE+0x0064)
#define PLL_IDN                  (PLL_BASE+0x0070)
#define XOSC32_AC_CON            (PLL_BASE+0x007C)
#define SLEEP_CON                (CONFIG_BASE+0x0204)

#define EMI_CONI (EMI_BASE + 0x40)        //DRAM MR/EMR
#define EMI_CONJ (EMI_BASE + 0x48)       // DRAM controller timing configuration I
#define EMI_CONK (EMI_BASE + 0x50)      // DRAM controller timing configuration II
#define EMI_CONL (EMI_BASE + 0x58)     // DRAM controller read data path configuration
#define EMI_CONM (EMI_BASE + 0x60)     // Digital DLL Control EMI_CONM
#define EMI_CONN (EMI_BASE + 0x68)
#define EMI_GENA (EMI_BASE + 0x70)
#define EMI_GENB (EMI_BASE + 0x78)
#define EMI_GENC (EMI_BASE + 0x0080) //EMI General Control Register C
#define EMI_GEND    (EMI_BASE + 0x0088) //EMI General Control Register D
#define EMI_GENE    (EMI_BASE + 0x0090) //Modem side offset address
#define EMI_DELA     (EMI_BASE + 0x0098) //EMI In/Out Delay Line Control
#define EMI_DELB     (EMI_BASE + 0x00A0) //EMI In/Out Delay Line Control
#define EMI_DELC     (EMI_BASE + 0x00A8) //EMI In/Out Delay Line Control
#define EMI_DELD     (EMI_BASE + 0x00B0) //EMI In/Out Delay Line Control
#define EMI_DELE     (EMI_BASE + 0x00B8) //EMI In/Out Delay Line Control
#define EMI_DELF     (EMI_BASE + 0x00C0) //EMI In/Out Delay Line Control
#define EMI_DELG     (EMI_BASE + 0x00C8) //EMI In/Out Delay Line Control
#define EMI_DELH     (EMI_BASE + 0x00D0) //EMI In/Out Delay Line Control
#define EMI_DELI     (EMI_BASE + 0x00D8) //EMI In/Out Delay Line Control
#define EMI_DELJ     (EMI_BASE + 0x00E0) //EMI In/Out Delay Line Control

#define EMI_DRCT     (EMI_BASE + 0x01A0) // EMI dummy read controls
#define EMI_DQSA     (EMI_BASE + 0x01B0) // DQSI auto-tracking control for CS[0]
#define EMI_DQSB     (EMI_BASE + 0x01B8) // DQSI auto-tracking control for CS[1]
#define EMI_DQSC     (EMI_BASE + 0x01C0) // DQSI auto-tracking control for CS[2]
#define EMI_DQSD     (EMI_BASE + 0x01C8) // DQSI auto-tracking control for CS[3]
#define EMI_DQSV     (EMI_BASE + 0x01D0) // DQSI auto-tracking calibrating delay value
#define EMI_MDCL     (EMI_BASE + 0x01E0) // Modem side control register

#define MT6516_WDT_BASE                 RGU_BASE

#define         MT6516_WDT_MODE         (MT6516_WDT_BASE+0x0000)
/*WDT_MODE*/
#define         MT6516_WDT_MODE_KEYMASK     0xff00
#define         MT6516_WDT_MODE_KEY         0x2200
#define         MT6516_WDT_MODE_ENABLE      0x0001

//RTC registers
#define RTC_BBPU        (RTC_BASE + 0x0000)
#define RTC_IRQ_STA     (RTC_BASE + 0x0004)
#define RTC_IRQ_EN      (RTC_BASE + 0x0008)
#define RTC_CII_EN      (RTC_BASE + 0x000c)
#define RTC_AL_MASK     (RTC_BASE + 0x0010)
#define RTC_TC_SEC      (RTC_BASE + 0x0014)
#define RTC_TC_MIN      (RTC_BASE + 0x0018)
#define RTC_TC_HOU      (RTC_BASE + 0x001c)
#define RTC_TC_DOM      (RTC_BASE + 0x0020)
#define RTC_TC_DOW      (RTC_BASE + 0x0024)
#define RTC_TC_MTH      (RTC_BASE + 0x0028)
#define RTC_TC_YEA      (RTC_BASE + 0x002c)
#define RTC_AL_SEC      (RTC_BASE + 0x0030)
#define RTC_AL_MIN      (RTC_BASE + 0x0034)
#define RTC_AL_HOU      (RTC_BASE + 0x0038)
#define RTC_AL_DOM      (RTC_BASE + 0x003c)
#define RTC_AL_DOW      (RTC_BASE + 0x0040)
#define RTC_AL_MTH      (RTC_BASE + 0x0044)
#define RTC_AL_YEA      (RTC_BASE + 0x0048)
#define RTC_XOSCCALI    (RTC_BASE + 0x004c)
#define RTC_POWERKEY1   (RTC_BASE + 0x0050)
#define RTC_POWERKEY2   (RTC_BASE + 0x0054)
#define RTC_PDN1        (RTC_BASE + 0x0058)
#define RTC_PDN2        (RTC_BASE + 0x005c)
#define RTC_SPAR1       (RTC_BASE + 0x0064)
#define RTC_PROT        (RTC_BASE + 0x0068)
#define RTC_DIFF        (RTC_BASE + 0x006c)
#define RTC_WRTGR       (RTC_BASE + 0x0074)

//RTC BBPU bits
#define RTC_BBPU_PWREN          (0x1 << 0)
#define RTC_BBPU_WRITE_EN       (0x1 << 1)
#define RTC_BBPU_BBPU           (0x1 << 2)
#define RTC_BBPU_AUTO           (0x1 << 3)
#define RTC_BBPU_CLRPKY         (0x1 << 4)
#define RTC_BBPU_RELOAD         (0x1 << 5)
#define RTC_BBPU_CBUSY          (0x1 << 6)
#define RTC_BBPU_DBING          (0x1 << 7)

//RTC IRQ status bits
#define RTC_IRQ_STA_ALSTA       (0x1 << 0)
#define RTC_IRQ_STA_TCSTA       (0x1 << 1)

//RTC IRQ enable bits
#define RTC_IRQ_EN_AL           (0x1 << 0)
#define RTC_IRQ_EN_TC           (0x1 << 1)
#define RTC_IRQ_EN_ONESHOT      (0x1 << 2)

//RTC Counter increment IRQ enable bits
#define RTC_CII_SEC             (0x1 << 0)
#define RTC_CII_MIN             (0x1 << 1)
#define RTC_CII_HOU             (0x1 << 2)
#define RTC_CII_DOM             (0x1 << 3)
#define RTC_CII_DOW             (0x1 << 4)
#define RTC_CII_MTH             (0x1 << 5)
#define RTC_CII_YEA             (0x1 << 6)
#define RTC_CII_12SEC           (0x1 << 7)
#define RTC_CII_14SEC           (0x1 << 8)
#define RTC_CII_18SEC           (0x1 << 9)

//RTC alarm mask bits
#define RTC_AL_SEC_MSK          (0x1 << 0)
#define RTC_AL_MIN_MSK          (0x1 << 1)
#define RTC_AL_HOU_MSK          (0x1 << 2)
#define RTC_AL_DOM_MSK          (0x1 << 3)
#define RTC_AL_DOW_MSK          (0x1 << 4)
#define RTC_AL_MTH_MSK          (0x1 << 5)
#define RTC_AL_YEA_MSK          (0x1 << 6)

//RTC PDN1
#define RTC_PDN1_2MS            (0x0 << 1)
#define RTC_PDN1_8MS            (0x1 << 1)
#define RTC_PDN1_32MS           (0x2 << 1)
#define RTC_PDN1_128MS          (0x3 << 1)
#define RTC_PDN1_256MS          (0x4 << 1)
#define RTC_PDN1_512MS          (0x5 << 1)
#define RTC_PDN1_1024MS         (0x6 << 1)
#define RTC_PDN1_2048MS         (0x7 << 1)

//RTC Enable the transfers from core to RTC in the queue
#define RTC_WRTGR_WRTGR         0x1

//RTC_POWERKEY
#define RTC_POWERKEY1_KEY       0xa357
#define RTC_POWERKEY2_KEY       0x67d2

/* we map HW YEA 0 (2000) to 1968 not 1970 because 2000 is the leap year */
#define RTC_MIN_YEAR            1968
#define RTC_NUM_YEARS           128
//#define RTC_MAX_YEAR          (RTC_MIN_YEAR + RTC_NUM_YEARS - 1)

#define RTC_PROT_KEY            0x9136
#define RTC_BBPU_KEY            (0x43 << 8)

#define DDR_PATTERN1   0x5555aaaa
#define DDR_PATTERN2   0x12345678

extern void store_8word(uint32_t a, uint32_t b);
extern uint32_t load_8word(uint32_t a, uint32_t b);

void lowlevel_init(void) {

}
int board_early_init_f(void) {

	uint16_t tmp;
	uint16_t bbpu;
	volatile uint32_t i;

	// disablewatchdog

	tmp = readw(MT6516_WDT_MODE);
	tmp &= ~MT6516_WDT_MODE_ENABLE; /* disable watchdog */
	tmp |= (MT6516_WDT_MODE_KEY); /* need key then write is allowed */
	writew(tmp, MT6516_WDT_MODE);
	//clrsetbits_le16(MT6516_WDT_MODE, MT6516_WDT_MODE_ENABLE, MT6516_WDT_MODE_KEY);
	// baseband on

	// pull PWRBB high
	bbpu = RTC_BBPU_KEY | RTC_BBPU_AUTO | RTC_BBPU_BBPU |
	RTC_BBPU_WRITE_EN | RTC_BBPU_PWREN;

	// unlock
	writew(RTC_PROT_KEY, RTC_PROT);
	writew(RTC_WRTGR_WRTGR, RTC_WRTGR);
	while (readw(RTC_BBPU) & RTC_BBPU_CBUSY)
		; /* 120 us */

	writew(bbpu, RTC_BBPU);
	writew(RTC_WRTGR_WRTGR, RTC_WRTGR);
	while (readw(RTC_BBPU) & RTC_BBPU_CBUSY)
		; /* 120 us */

	// lock
	writew(0, RTC_PROT);
	writew(RTC_WRTGR_WRTGR, RTC_WRTGR);
	while (readw(RTC_BBPU) & RTC_BBPU_CBUSY)
		; /* 120 us */

	// Switch to pll

	writew(0x340, SLEEP_CON);
	writew(0x1e, PDN_CON); // power-on MPLL(DPLL), UPLL
	writew(0x1f, CEVAPLL2); // power-on CEVAPLL
	writew(0x83, CLK_CON); // switch to 13MHz for PLL input frequency

	// After power-on PLL.....
	writew(0x0080, UPLL);  // reset UPLL
	writew(0x0080, MPLL);  // reset MPLL
	writew(0x0800, CEVAPLL); // reset CEVAPLL

	writew(0x0000, UPLL);  // release UPLL reset
	writew(0x0000, MPLL);  // release MPLL reset
	writew(0x0000, CEVAPLL); // release CEVAPLL reset

	// some delay for PLL setup
	for (i = 0; i < 400 * 4; i++) {
		i;
	}

	writew(0x00f3, CLK_CON); // select PLL outputs
	return 0;
}

int board_init(void) {
	debug("board_init mt6516\n");
	return 0;
}

// Enable PLL before calling dram_init

int dram_init(void) {
	debug("dram init mt6516\n");
	uint32_t reg;
	uint8_t dqsdly_tbl[16] = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38,
			0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78 };

	uint32_t i, j;
	uint32_t dram_baseaddr;
	uint32_t dqs_tbl_size = sizeof(dqsdly_tbl) / sizeof(uint8_t);
	uint32_t dqs_offset;
	uint32_t valid_count, tmp;

	// config 1 DRAM bank,
	writel(0x1000E, EMI_GEND);

	// switch to external boot and enable dram clock
	// enable HW DRAM self-refresh mode
	writel(0x30A, EMI_GENA);

	// config DRAM type and address type and enable DRAM controller
	// EMI_CONN_Value 		0x00530001
	writel(0x630013, EMI_CONN);

	// DRAM AC timing setting
	// using a TOSHIBA DRAM  TYBC0A111086KC
	writel(0x314040, EMI_CONI);
	writel(0x9692520, EMI_CONJ);
	writel(0xC0000, EMI_CONK);
	writel(0x13004, EMI_CONL);

	// DRAM driving
	writel(0xAAAAA0, EMI_GENB);
	writel(0xAAAA0A, EMI_GENC);

	// Setup/Hold time balance
	// Setup time adjustment: DQ_OUT and DQM_OUT (delay data to shorten setup time)
	// Hold time adjustment: DQS_OUT and EDCLK_OUT (delay DQS to shorten hold time)
	writel(0, EMI_DELA);
	writel(0, EMI_DELB);
	writel(0, EMI_DELC);
	writel(0, EMI_DELD);
	writel(0x44440000, EMI_DELI);
	writel(0x14000000, EMI_DELJ);

	// DRAM init procedure

	// pre-charge all
	reg = readl(EMI_CONN);
	reg |= 0x10000000;
	writel(reg, EMI_CONN);
	reg = readl(EMI_CONN);
	reg &= ~(0x10000000);
	writel(reg, EMI_CONN);

	// auto-refresh 1
	reg = readl(EMI_CONN);
	reg |= 0x8000000;
	writel(reg, EMI_CONN);
	reg = readl(EMI_CONN);
	reg &= ~(0x8000000);
	writel(reg, EMI_CONN);

	// auto-refresh 2
	reg = readl(EMI_CONN);
	reg |= 0x4000000;
	writel(reg, EMI_CONN);
	reg = readl(EMI_CONN);
	reg &= ~(0x4000000);
	writel(reg, EMI_CONN);

	// set mode register
	reg = readl(EMI_CONN);
	reg |= 0x2000000;
	writel(reg, EMI_CONN);
	reg = readl(EMI_CONN);
	reg &= ~(0x2000000);
	writel(reg, EMI_CONN);

	// set extended mode register
	reg = readl(EMI_CONN);
	reg |= 0x1000000;
	writel(reg, EMI_CONN);
	reg = readl(EMI_CONN);
	reg &= ~(0x1000000);
	writel(reg, EMI_CONN);

	// enable 3.25MHz fixed-clock DRAM auto-refresh
	reg = readl(EMI_CONN);
	reg |= 0x4;
	writel(reg, EMI_CONN);

	// enable DDR 1/5T Digital DDL lock
	reg = readl(EMI_CONN);
	reg |= 0x100;     // Enable auto refresh to DRAM
	writel(reg, EMI_CONN);

	// Enable power down mode in CONN for power saving
	reg = readl(EMI_CONN);
	reg |= 0x10;
	writel(reg, EMI_CONN);

	// High priority for MD (AHB3)
	reg = readl(EMI_GENA);
	reg |= 0x1000;
	writel(reg, EMI_GENA);

	// reset and disable all DRAM bank auto-tracking first
	writel(0x0, EMI_DQSA);
	writel(0x0, EMI_DQSB);
	writel(0x0, EMI_DQSC);
	writel(0x0, EMI_DQSD);

	// process all DRAM bank
	for (i = 0; i < 2; i++) {

		if (0 == (readl(EMI_GEND) & (0x10000 << i))) {
			// bank is disabled
			continue;
		}

		dram_baseaddr = 0x10000000 * i;

		// DQS auto-tracking initial value calibration
		valid_count = 0;
		store_8word(dram_baseaddr, DDR_PATTERN1); // write PATTERN1 for clearing EMI read FIFO
		for (j = dqs_tbl_size - 1; j >= 0; j--) {
			// set DQS auto-tracking offset
			dqs_offset = (dqsdly_tbl[j] << 24) | (dqsdly_tbl[j] << 16)
					| (dqsdly_tbl[j] << 8) | dqsdly_tbl[j];

			if (0 == i) {
				//*EMI_DQSA = dqs_offset;
				writel(dqs_offset, EMI_DQSA);
			} else if (1 == i) {
				//*EMI_DQSB = dqs_offset;
				writel(dqs_offset, EMI_DQSB);
			} else if (2 == i) {
				//*EMI_DQSC = dqs_offset;
				writel(dqs_offset, EMI_DQSC);
			} else if (3 == i) {
				//*EMI_DQSD = dqs_offset;
				writel(dqs_offset, EMI_DQSD);
			}

			if (0 == valid_count) {
				// read back from DDR and compare with PATTERN1
				if (0 == load_8word(dram_baseaddr, DDR_PATTERN1)) {
					// EMI read FIFO is PATTERN1 now, write PATTERN2 for auto-tracking initial offset calibration
					store_8word(dram_baseaddr, DDR_PATTERN2);
					valid_count++;
				}
			}
			if (1 == valid_count) {
				// read back from DDR and compare with PATTERN2
				if (0 == load_8word(dram_baseaddr, DDR_PATTERN2)) {
					// PATTERN2 is matched, in order to skip bounary offset value, set to next DQS offset and read again
					valid_count++;
				}
			}

			if (2 == valid_count) {
				// read back from DDR and compare with PATTERN2
				if (0 == load_8word(dram_baseaddr, DDR_PATTERN2)) {
					// a valid initial offset is found!
					debug("valid DQS offset: %x\n", (uint32_t)dqs_offset);
					break;
				} else {
					// error, should not happen!
					debug("no valid DQS offset\n");
					BUG();


				}
			}
		}
		// enable auto-tracking for bank
		if (0 == i) {

			tmp = readl(EMI_DQSA);
			tmp |= 0x80808080;
			writel(tmp, EMI_DQSA);
		} else if (1 == i) {
			tmp = readl(EMI_DQSB);
			tmp |= 0x80808080;
			writel(tmp, EMI_DQSB);
		} else if (2 == i) {
			tmp = readl(EMI_DQSC);
			tmp |= 0x80808080;
			writel(tmp, EMI_DQSC);
		} else if (3 == i) {
			tmp = readl(EMI_DQSD);
			tmp |= 0x80808080;
			writel(tmp, EMI_DQSD);
		}
	}
	// enable dummy read
	writel(0x223, EMI_DRCT);

	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = get_ram_size((long *) CONFIG_SYS_SDRAM_BASE,
			PHYS_SDRAM_1_SIZE);
	debug("Ramsize %08lx\n", gd->ram_size);

	return 0;
}

#if defined(CONFIG_SYS_DRAM_TEST)
int testdram (void)
{
	uint32_t *pstart = (uint32_t *) 0;
	uint32_t *pend = (uint32_t *) PHYS_SDRAM_1_SIZE;
	uint32_t *p;

	debug("SDRAM test phase 1:\n");
	for (p = pstart; p < pend; p++)
		*p = 0xaaaaaaaa;

	for (p = pstart; p < pend; p++) {
		if (*p != 0xaaaaaaaa) {
			debug ("SDRAM test fails at: %08x\n", (uint) p);
			return 1;
		}
	}

	debug("SDRAM test phase 2:\n");
	for (p = pstart; p < pend; p++)
		*p = 0x55555555;

	for (p = pstart; p < pend; p++) {
		if (*p != 0x55555555) {
			debug ("SDRAM test fails at: %08x\n", (uint) p);
			return 1;
		}
	}

	debug("SDRAM test passed.\n");
	return 0;
}
#endif

int board_late_init(void) {
	return 0;
}
