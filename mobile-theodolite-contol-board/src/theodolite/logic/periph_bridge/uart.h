/*
 * uart.h
 *
 *      Author: mmalenko
 * (c) 2016 NIC SPbETU
 */

#ifndef PERIPHERAL_UART_UART_H_
#define PERIPHERAL_UART_UART_H_

#include "firmware/interfaces/interfaces.h"

void uartConfig();

ConnectionInterface &getCameraConnectionInterface();
ConnectionInterface &getEncoderConnectionInterface();

#endif /* PERIPHERAL_UART_UART_H_ */
