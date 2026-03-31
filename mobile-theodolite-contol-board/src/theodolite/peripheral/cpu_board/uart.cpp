/*
 * uart.c
 *
 *      Author: mmalenko
 *  (c) 2016 NIC SPbETU
 */

#include <theodolite/peripheral/hardware.h>
#include <theodolite/peripheral/common/pc_uart/pc_uart.h>

const uint32_t CPU_BOARD_UART_BAUDRATE = 115200;

void uartConfig()
{
  pcUartConfig(CPU_BOARD_UART_BAUDRATE);
}

