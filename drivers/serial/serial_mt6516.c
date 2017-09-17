

#include <common.h>
#include <watchdog.h>
#include <asm/io.h>
#include <serial.h>
#include <linux/compiler.h>
#include <asm/arch/mt6516.h>
#include <dm/platform_data/serial_mt6516.h>

#define MT6516_UART1_BASE               (0x80023000)
#define MT6516_UART2_BASE               (0x80024000)
#define MT6516_UART3_BASE               (0x80025000)
#define MT6516_UART4_BASE               (0x8002B000)



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

static const struct mt6516_serial_platdata serial_platdata = {
	.base = (struct mt6516_usart *)MT6516_UART1_BASE,
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


static void
mt6516_serial_setbrg(void)
{
  unsigned int byte;
  unsigned int highspeed;
  unsigned int quot, divisor, remainder;
  unsigned int uartclk;
  uint32_t brg=   115200;

  struct mt6516_usart *const usart = serial_platdata.base;

  brg = gd->baudrate;

  // 52 Mhz
  uartclk = EMI_104MHZ >> 1;

  if (brg <= 115200)
    {
      highspeed = 0;
      quot = 16;
    }
  else
    {
      highspeed = 2;
      quot = 4;
    }

  /* Set divisor DLL and DLH  */
  divisor = uartclk / (quot * brg);
  remainder = uartclk % (quot * brg);

  if (remainder >= (quot / 2) * brg)
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


static int
mt6516_serial_getc(void)
{
  struct mt6516_usart *const usart = serial_platdata.base;
  while (!(readl(&usart->uart_lsr) & UART_LSR_DR))
    {
      WATCHDOG_RESET();
    }
  return (int) readl(&usart->uart_rbr);
}

static void
mt6516_serial_putc(const char c)
{
	struct mt6516_usart *const usart = serial_platdata.base;
	while (!(readl(&usart->uart_lsr) & UART_LSR_THRE))
    ;

  if (c == '\n')
	// UART_THR
    writel((unsigned int) '\r', &usart->uart_rbr);
  // UART_THR
  writel((unsigned int) c, &usart->uart_rbr);
}

static void
mt6516_serial_puts(const char *s)
{
  while (*s)
    serial_putc(*s++);
}

/*
 * Test whether a character is in the RX buffer
 */
static int
mt6516_serial_tstc(void)
{
	struct mt6516_usart *const usart = serial_platdata.base;
	return readw(&usart->uart_lsr) & UART_LSR_DR;
}

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */

static int
mt6516_serial_init(void)
{
  uint16_t reg;
  struct mt6516_usart *const usart = serial_platdata.base;
  // Power on UART 4
  writel((1 << PDN_PERI_UART4), APMCUSYS_PDN_CLR0);

  // UART_FCR
  setbits_le32(&usart->uart_iir,UART_FCR_FIFO_INIT); /* clear fifo */

  writew(UART_NONE_PARITY | UART_WLS_8 | UART_1_STOP, &usart->uart_lcr);

  // set PINs for UART4
  reg= readw(GPIO_MODE_BASE + 0x80);
  reg &= ~(0x03 << (2*5));
  reg |= (0x01 << (2*5));

  reg &= ~(0x03 << (2*6));
  reg |= (0x01 << (2*6));

  writew(reg,GPIO_MODE_BASE + 0x80);

  // set UART4 to Application Processor
  reg= readw(HW_MISC);
  reg &= ~(1 << 6);
  writew(reg,HW_MISC);

  mt6516_serial_setbrg();
  return 0;
}

static struct serial_device mt6516_serial_drv =
  {
      .name = "mt6516",
      .start = mt6516_serial_init,
      .stop = NULL,
      .setbrg = mt6516_serial_setbrg,
      .putc = mt6516_serial_putc,
      .puts = mt6516_serial_puts,
      .getc = mt6516_serial_getc,
      .tstc = mt6516_serial_tstc, };

void
mt6516_serial_initialize(void)
{
  serial_register(&mt6516_serial_drv);
}

__weak struct serial_device *
default_serial_console(void)
{
  return &mt6516_serial_drv;
}
