/*
 * mnp_binary.h
 *
 *  Created on: 6 апр. 2016 г.
 *      Author: mmalenko
 *  (c) 2015 NIC SPbETU
 */

#ifndef SOURCE_LOGIC_GNSS_MNP_BINARY_H_
#define SOURCE_LOGIC_GNSS_MNP_BINARY_H_

#include "stdint.h"
#include "firmware/ring_buffer/ring_buffer.h"

/*
 *
 *|SYNC_WORD|MNP_XX_ID|DATA_SIZE|RESERVED_WORD|CRC|
 * */

const uint16_t SYNC_WORD = 0x81FF;
const uint16_t MNP_MODE_ID = 0x0BBE; //3006
const uint16_t MNP_MODE_ID_CONFIG_CMD_CODE = 0x0200; //configuration words request
const uint16_t RESERVED_WORD = 0x0;
const uint16_t MNP_MODE_ID_CONFIG_CMD_DATA_SIZE = 0x2; //configuration words request data size
const uint16_t MNP_MODE_ID_CONFIG_WRITE_CMD_DATA_SIZE = 0xE; //configuration words request data size

#define MNP_BINARY_HEADER_SIZE   5
#define MNP_CONFIG_WORDS_NUMBER  6

const uint16_t MNP_BINARY_CMD_RAM = 0x01;
const uint16_t MNP_BINARY_CMD_FLASH = 0x04;
const uint16_t MNP_BINARY_CMD_WRITE = 0x80;
const uint16_t MNP_BINARY_CMD_READ = 0x00;

#define CFG_CMD_CHANNEL_PROTOCOL_MNP_BINARY     0x1
#define CFG_CMD_CHANNEL_PROTOCOL_NMEA           0x8
#define CFG_CMD_CHANNEL_DIV_SHIFT               22
#define CHANNEL_BOUDRATE_CONST                  ((uint32_t)460800)

#define CONFIG_CMD_LEN        40
#define MNP_HEADER_SIZE       5//16b

#define MNP_HEADER_CRC_SHIFT  8//8b
#define MNP_DATA_SIZE_SHIFT   4//8b
#define MNP_ID_SHIFT          2//8b

#define CFG_CMD_CH0_SHIFT     1
#define CFG_CMD_CH1_SHIFT     2

#define CFG_CMD_PROTOCOL_CLEAR_MASK     (~(uint32_t)(0xFF))
#define CFG_CMD_DIV_CLEAR_MASK          (~(((uint32_t)0x3FF)<<CFG_CMD_CHANNEL_DIV_SHIFT))
#define GLO_TIME_SET_MASK               ((uint32_t)0x20000)
#define GLO_TIME_RESET_MASK             (~GLO_TIME_SET_MASK)
#define SYS_TIME_SET_MASK               ((uint32_t)0x10000)
#define SYS_TIME_RESET_MASK             (~SYS_TIME_SET_MASK)

#define GGA_MSG_SET_MASK               ((uint32_t)0x1)
#define GGA_MSG_RESET_MASK             (~GGA_MSG_SET_MASK)
#define GSA_MSG_SET_MASK               ((uint32_t)0x2)
#define GSA_MSG_RESET_MASK             (~GSA_MSG_SET_MASK)
#define GSV_MSG_SET_MASK               ((uint32_t)0x4)
#define GSV_MSG_RESET_MASK             (~GSV_MSG_SET_MASK)
#define RMC_MSG_SET_MASK               ((uint32_t)0x8)
#define RMC_MSG_RESET_MASK             (~RMC_MSG_SET_MASK)

#define CH1_MSG_RESET_MASK             (0xFFFFF0FF)

typedef enum
{
  MNP_BINARY,
  NMEA
} GnssProtocol;

typedef struct
{
  GnssProtocol uart0Protocol;
  uint32_t uart0Boudrate;
  GnssProtocol uart1Protocol;
  uint32_t uart1Boudrate;
  uint8_t GLO_TIME; //0 1
  uint8_t SYS_TIME; //0 1
  uint8_t CH0_RMC_ENABLE; //0 1
  uint8_t CH0_GGA_ENABLE; //0 1
  uint8_t CH0_GSV_ENABLE; //0 1
  uint8_t CH0_GSA_ENABLE; //0 1
  uint16_t source; //MNP_BINARY_CMD_RAM;MNP_BINARY_CMD_FLASH

} GnssConfigStruct;

#define IS_CONFIGURATION_REQUEST_SOURCE(source)   ((source == MNP_BINARY_CMD_RAM)||(source == MNP_BINARY_CMD_FLASH))
#define IS_CONFIGURATION_REQUEST_MODE(mode)       ((mode == MNP_BINARY_CMD_WRITE)||(mode == MNP_BINARY_CMD_READ))
#define IS_PROTOCOL(protocol)                     ((protocol==MNP_BINARY) ||(protocol==NMEA))

void mnpSendReadConfigurationRequest(uint16_t source);
void gnssConfigurationSetting(GnssConfigStruct *initialStruct, uint32_t *configurationData);
int8_t mnpReadConfigurationData(uint32_t *data);
RingBuffer* getGnssRxRingBuffer(void);
RingBuffer* getGnssMnpBinaryTxRingBuffer(void);

#endif /* SOURCE_LOGIC_GNSS_MNP_BINARY_H_ */
