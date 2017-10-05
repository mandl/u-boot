#include <common.h>
#include <dm.h>
#include <watchdog.h>
#include <asm/io.h>
#include <serial.h>


#include <asm/arch/mt6516.h>
#include <dm/platform_data/serial_mt6516.h>

struct mt6516_usart {

	uint32_t uart_rbr;                   // 0x0)  Read only
//uint32_t uart_thr;                 // 0x0)  Write only

	uint32_t uart_ier;                   // 0x4)
	uint32_t uart_iir;                   // 0x8)  Read only
//uint32_t uart_fcr;                 // 0x8)  Write only
	uint32_t uart_lcr;                   // 0xc)
	uint32_t uart_mcr;                   // 0x10)
	uint32_t uart_lsr;                   // 0x14)
	uint32_t uart_msr;                   // 0x18)
	uint32_t uart_scr;                   // 0x1c)

//uint32_t uart_dll;                   // 0x0)   Only when LCR.DLAB = 1
//uint32_t uart_dlh;                   // 0x4)   Only when LCR.DLAB = 1

//uint32_t uart_efr;                   // 0x8)   Only when LCR = 0xbf
//uint32_t uart_xon1;                  // 0x10)  Only when LCR = 0xbf
//uint32_t uart_xon2;                  // 0x14)  Only when LCR = 0xbf
//uint32_t uart_xoff1;                 // 0x18)  Only when LCR = 0xbf
//uint32_t uart_xoff2;                 // 0x1c)  Only when LCR = 0xbf

	uint32_t uart_autobaud_en;           // 0x20)
	uint32_t uart_highspeed;             // 0x24)
	uint32_t uart_sample_count;          // 0x28)
	uint32_t uart_sample_point;          // 0x2c)
	uint32_t uart_autobaud_reg;          // 0x30)
	uint32_t uart_rate_fix_ad;           // 0x34)
	uint32_t uart_autobaud_sample;       // 0x38)
	uint32_t uart_guard;                 // 0x3c)
	uint32_t uart_escape_dat;            // 0x40)
	uint32_t uart_escape_en;             // 0x44)
	uint32_t uart_sleep_en;              // 0x48)
	uint32_t uart_vfifo_en;              // 0x4c)
	uint32_t uart_rxtri_ad;              // 0x50)
};



#define UART_NONE_PARITY            (0 << 3)
#define UART_WLS_8                  (3 << 0)
#define UART_1_STOP                 (0 << 2)
#define UART_NONE_PARITY            (0 << 3)

#define UART_LSR_THRE               (1 << 5)
#define UART_LCR_DLAB               (1 << 7)

/* LSR */
#define UART_LSR_DR                 (1 << 0)

#define UART_FCR_FIFOE              (1 << 0)
#define UART_FCR_CLRR               (1 << 1)
#define UART_FCR_CLRT               (1 << 2)

#define UART_FCR_FIFO_INIT          (UART_FCR_FIFOE|UART_FCR_CLRR|UART_FCR_CLRT)

DECLARE_GLOBAL_DATA_PTR;

void mt6516_serial_init(struct mt6516_usart *usart) {
	uint16_t reg;

	// enable UART 4 clock
	writel((1 << PDN_PERI_UART4), APMCUSYS_PDN_CLR0);

	// UART_FCR
	setbits_le32(&usart->uart_iir, UART_FCR_FIFO_INIT); /* clear fifo */

	writew(UART_NONE_PARITY | UART_WLS_8 | UART_1_STOP, &usart->uart_lcr);

	// set PINs for UART4
	reg = readw(GPIO_MODE_BASE + 0x80);
	reg &= ~(0x03 << (2 * 5));
	reg |= (0x01 << (2 * 5));

	reg &= ~(0x03 << (2 * 6));
	reg |= (0x01 << (2 * 6));

	writew(reg, GPIO_MODE_BASE + 0x80);

	// set UART4 to Application Processor
	reg = readw(HW_MISC);
	reg &= ~(1 << 6);
	writew(reg, HW_MISC);
}

void mt6516_serial_baud(struct mt6516_usart *usart, int baudrate) {
	unsigned int byte;
	unsigned int highspeed;
	unsigned int quot, divisor, remainder;
	unsigned int uartclk;

	// 52 Mhz
	uartclk = EMI_104MHZ >> 1;

	if (baudrate <= 115200) {
		highspeed = 0;
		quot = 16;
	} else {
		highspeed = 2;
		quot = 4;
	}

	/* Set divisor DLL and DLH  */
	divisor = uartclk / (quot * baudrate);
	remainder = uartclk % (quot * baudrate);

	if (remainder >= (quot / 2) * baudrate)
		divisor += 1;

	writew(highspeed, &usart->uart_highspeed);

	byte = readl(&usart->uart_lcr); /* DLAB start */
	writel((byte | UART_LCR_DLAB), &usart->uart_lcr);

	// write DLL
	writel((divisor & 0x00ff), &usart->uart_rbr);

	// write DLH
	writel(((divisor >> 8) & 0x00ff), &usart->uart_ier);
	writel(byte, &usart->uart_lcr); /* DLAB end */

}



static int mt6516_serial_setbrg(struct udevice *dev, int baudrate) {

	baudrate = 115200;
	struct mt6516_serial_platdata *plat = dev->platdata;
	struct mt6516_usart * const usart = plat->base;
	uint32_t clk;

	clk = dev_read_u32_default(dev, "clock-frequency",52000000);
	debug("mt6516 clock %d\n",clk);
	mt6516_serial_baud(usart, baudrate);

	return 0;
}

static int mt6516_serial_getc(struct udevice *dev) {
	struct mt6516_serial_platdata *plat = dev->platdata;
	struct mt6516_usart * const usart = plat->base;

	if ((readl(&usart->uart_lsr) & UART_LSR_DR) == 0)
		return -EAGAIN;
	return (int) readl(&usart->uart_rbr);
}

static int mt6516_serial_putc(struct udevice *dev, const char c) {
	struct mt6516_serial_platdata *plat = dev->platdata;
	struct mt6516_usart * const usart = plat->base;
	if ((readl(&usart->uart_lsr) & UART_LSR_THRE) == 0)
		return -EAGAIN;

	writel((unsigned int) c, &usart->uart_rbr);
	return 0;
}

/*
 * Test whether a character is pending
 */
static int mt6516_serial_pending(struct udevice *dev, bool input) {
	struct mt6516_serial_platdata *plat = dev->platdata;
	struct mt6516_usart * const usart = plat->base;

	if (input)
		return readw(&usart->uart_lsr) & UART_LSR_DR ? 1 : 0;
	else
		return readl(&usart->uart_lsr) & UART_LSR_THRE ? 0 : 1;
}

/*
 * Initialize the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */

static int mt6516_serial_probe(struct udevice *dev) {

	struct mt6516_serial_platdata *plat = dev->platdata;
	struct mt6516_usart * const usart = plat->base;

	mt6516_serial_init(usart);

	return 0;
}

static int mt6516_serial_ofdata_to_platdata(struct udevice *dev)
{
	struct mt6516_serial_platdata *plat = dev->platdata;
	fdt_addr_t addr = devfdt_get_addr(dev);;

	debug("!!!!!!!!!!!!!!!!!mt6516 addr %x\n",addr);


	plat->base = (struct mt6516_usart *)MT6516_UART4_BASE;
	//plat->base = (struct mt6516_usart *)addr;
//	plat->port_id = fdtdec_get_int(gd->fdt_blob, dev_of_offset(dev),
//					"id", dev->seq);
	return 0;
}

static const struct udevice_id mt6516_serial_ids[] = {
	{ .compatible = "mt6516" },
	{ }
};

static const struct dm_serial_ops mt6516_serial_ops = {  // driver model
		        .putc = mt6516_serial_putc,        // put char
				.pending = mt6516_serial_pending,  // input or output pending
				.getc = mt6516_serial_getc,        // get char
				.setbrg = mt6516_serial_setbrg, }; // set baud

U_BOOT_DRIVER(serial_mt6516) = {

	.name = "serial_mt6516",
	.id = UCLASS_SERIAL,
	.of_match = mt6516_serial_ids,
	.ofdata_to_platdata = mt6516_serial_ofdata_to_platdata,
	.ops = &mt6516_serial_ops,
	.probe = mt6516_serial_probe,
	.flags = DM_FLAG_PRE_RELOC,
	.platdata_auto_alloc_size = sizeof(struct mt6516_serial_platdata),

};

#ifdef DEBUG_UART_MT6516

#include <debug_uart.h>

static inline void _debug_uart_init(void) {
	struct mt6516_usart *uart = (struct mt6516_usart *) MT6516_UART4_BASE;

	mt6516_serial_init(uart);

	mt6516_serial_baud(uart, 115200);

}

static inline void _debug_uart_putc(int ch) {
	struct mt6516_usart *uart = (struct mt6516_usart *) MT6516_UART4_BASE;

	while (!(readl(&uart->uart_lsr) & UART_LSR_THRE)) {
	}

	writel((unsigned int)ch, &uart->uart_rbr);
}

DEBUG_UART_FUNCS

#endif

