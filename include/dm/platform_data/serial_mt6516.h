/*
 * (C) Copyright 2017
 *
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __SERIAL_MT6516_H
#define __SERIAL_MT6516_H

/* Information about a serial port */


struct mt6516_serial_platdata {
	struct mt6516_usart *base; /* address of registers in physical memory */
};

#endif /* __SERIAL_MT6516_H */
