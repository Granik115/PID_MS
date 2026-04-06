/* $Id: cpuprotocol.h dpshenichnov  */
/*!
 * \file cpuprotocol.h
 * \brief
 *
 * PROJ: BFT
 *
 * (c) 2015 NIC SPbETU
 * ------------------------------------------------------------------------ */
#ifndef CPUPROTOCOL_H_
#define CPUPROTOCOL_H_

#include "firmware/link_level/uart_link.h"
#include "bft.pb.h"
#include "time_module/time/porttime.h"


typedef enum
{
  UART_CHANNEL1 = 0,
  UART_CHANNEL2 = 1,
  UART_CHANNEL3 = 2,
  UART_CHANNEL4 = 3,
} TimeInterface;


void sendTime(const PulseTime&, TimeInterface);
void protocolTask(void);

#endif /* CPUPROTOCOL_H_ */
