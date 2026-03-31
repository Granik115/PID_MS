/*
 * uart.h
 *
 *  Created on: 16 мая 2016 г.
 *      Author: mmalenko
 *       (c) 2015 NIC SPbETU
 */

#ifndef PERIPHERAL_UART_H_
#define PERIPHERAL_UART_H_

#include "hal.h"
#include "uart_link.h"
#include "bft.pb.h"


#define SYNC_CODE_BAUDRATE 230400

typedef enum
{
  GNSS_NMEA_AND_SYNC_OUT_REMAP,
  GNSS_MNP_BINARY_REMAP
} SyncGnssUartRemap;

typedef struct
{
  bool isDmaTaskRead = true;
  uint32_t size;
  uint8_t data[100];
} SyncChannelDecstiptor;

bool dmaTask();
SyncChannelDecstiptor* getChannelsDescriptors();
void mnpBinaryUartSendByteBlocking(uint8_t byte);
bool isMnpBinaryUartTxBusy();
void setDigitalSevInterface(timesync_messages_DigitalSevInterfaceType interface);
void digitalSevUartConfiguration(void);
void syncUartConfig(void);
void gnssMkUartConfiguration(uint32_t boudrate, SyncGnssUartRemap remap);
void setGnssSyncUartRemap(SyncGnssUartRemap remap);
bool uartSendData(Data *data, TimeInterface interface);
timesync_messages_DigitalSevInterfaceType getDigitalSevInterface(void);

#endif /* PERIPHERAL_UART_H_ */
