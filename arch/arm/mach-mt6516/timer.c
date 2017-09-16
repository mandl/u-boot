
#include <common.h>
#include <asm/arch/mt6516.h>
#include <asm/io.h>

#define TIMER_LOAD_VAL 0xffffffff

/* macro to read the 32 bit timer */
#define READ_TIMER __raw_readl(MT6516_XGPT1_COUNT)

DECLARE_GLOBAL_DATA_PTR;

static ulong timestamp = 0;
static ulong lastinc;

int timer_init(void) {

	// Power on XGPT
	writel(1 << PDN_PERI_XGPT, APMCUSYS_PDN_CLR0);

	// Disable the XGTP to avoid the hardware runing and ack the old interrupt indication.
	writel(0x32, MT6516_XGPT1_CON);
	writel(0x01, MT6516_XGPT_IRQACK);

	// Disable Interrupt of XGPT
	writel(0x00, MT6516_XGPT_IRQEN);

	// Specify the resolution  812.5K
	writel(0x4, MT6516_XGPT1_PRESCALE);

	writel(0x33, MT6516_XGPT1_CON);

	// init the timestamp and lastdec value
	reset_timer_masked();

	return 0;
}

/* delay x useconds AND preserve advance timestamp value */
void __udelay(unsigned long usec) {
	ulong tmo, tmp;

	if (usec >= 1000) { /* if "big" number, spread normalization to seconds */
		tmo = usec / 1000; /* start to normalize for usec to ticks per sec */
		tmo *= CONFIG_SYS_HZ; /* find number of "ticks" to wait to achieve target */
		tmo /= 1000; /* finish normalize. */
	} else { /* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ;
		tmo /= (1000 * 1000);
	}

	tmp = get_timer(0); /* get current timestamp */
	if ((tmo + tmp + 1) < tmp) /* if setting this fordward will roll time stamp */
		reset_timer_masked(); /* reset "advancing" timestamp to 0, set lastdec value */
	else
		tmo += tmp; /* else, set advancing stamp wake up time */

	while (get_timer_masked() < tmo)
		/* loop till event */
		/*NOP*/;
}

void reset_timer_masked(void) {
	lastinc = READ_TIMER;
	timestamp = 0;
}

ulong get_timer_masked(void) {
	ulong now = READ_TIMER;

	if (now >= lastinc) {
		timestamp += now - lastinc; /* normal */
	} else {
		timestamp += TIMER_LOAD_VAL - lastinc + now; /* overflow */
	}
	lastinc = now;

	return timestamp;
}

/* waits specified delay value and resets timestamp */
void udelay_masked(unsigned long usec) {
	ulong tmo;
	ulong endtime;
	signed long diff;

	if (usec >= 1000) { /* if "big" number, spread normalization to seconds */
		tmo = usec / 1000; /* start to normalize for usec to ticks per sec */
		tmo *= CONFIG_SYS_HZ; /* find number of "ticks" to wait to achieve target */
		tmo /= 1000; /* finish normalize. */
	} else { /* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ;
		tmo /= (1000 * 1000);
	}

	endtime = get_timer_masked() + tmo;

	do {
		ulong now = get_timer_masked();
		diff = endtime - now;
	} while (diff >= 0);
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void) {
	return get_timer_masked();
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void) {
	ulong tbclk;

	tbclk = CONFIG_SYS_HZ;
	return tbclk;
}
