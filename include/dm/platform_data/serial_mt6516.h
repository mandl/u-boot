/*
 * (C) Copyright 2017
 *
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __SERIAL_MT6516_H
#define __SERIAL_MT6516_H

/* Information about a serial port */

#define MT6516_UART1_BASE               (0x80023000)
#define MT6516_UART2_BASE               (0x80024000)
#define MT6516_UART3_BASE               (0x80025000)
#define MT6516_UART4_BASE               (0x8002B000)

struct mt6516_serial_platdata {
	struct mt6516_usart *base; /* address of registers in physical memory */
};

#endif /* __SERIAL_MT6516_H */
