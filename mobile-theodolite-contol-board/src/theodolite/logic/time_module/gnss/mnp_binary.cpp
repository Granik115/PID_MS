/*
 * mnp_binary.c
 *
 *  Created on: 6 апр. 2016 г.
 *      Author: mmalenko
 *  (c) 2015 NIC SPbETU
 */

#include "time_module/gnss/mnp_binary.h"
#include "time_module/assert/assert_parameter.h"

uint16_t mnpCrc(uint16_t *data, uint8_t len)
{
  uint16_t summ = 0;
  for (uint8_t i = 0; i < len; i++)
  {
    summ += *(data + i);
  }
  return 0xFFFF - summ + 1;
}

uint16_t mnpCrcFromRingBuffer(RingBuffer *buffer, uint8_t len)
{
  uint16_t summ = 0;
  for (uint8_t i = 0; i < len; i++)
  {
    uint16_t word = showSymbolFromRingBuffer(i * 2, buffer);
    word |= ((uint16_t)showSymbolFromRingBuffer(i * 2 + 1, buffer)) << 8;
    summ += word;
  }
  return 0xFFFF - summ + 1;
}

static RingBuffer mnpBinaryTxBuffer;
static RingBuffer mnpBinaryRxBuffer;

RingBuffer* getGnssRxRingBuffer(void)
{
  return &mnpBinaryRxBuffer;
}

RingBuffer* getGnssMnpBinaryTxRingBuffer(void)
{
  return &mnpBinaryTxBuffer;
}

void mnpBinaryOutLittleEndian(uint16_t *data, uint8_t size)
{
  for (uint8_t i = 0; i < size; i++)
  {
    putSymbolToRingBuffer((uint8_t)(*(data + i)), &mnpBinaryTxBuffer);
    putSymbolToRingBuffer((uint8_t)(*(data + i) >> 8), &mnpBinaryTxBuffer);
  }
}

/*Sending read configuration  request
 *
 * source:   CMD_RAM
 *           CMD_FLASH
 * mode:      CMD_WRITE
 *           CMD_READ
 *
 * */
void mnpSendWriteConfigurationRequest(uint16_t source, uint32_t *configurationData)
{
  assert_parameter(IS_CONFIGURATION_REQUEST_SOURCE(source));
  uint16_t configRequestHeader[MNP_BINARY_HEADER_SIZE] =
  {SYNC_WORD,
    MNP_MODE_ID, MNP_MODE_ID_CONFIG_WRITE_CMD_DATA_SIZE, RESERVED_WORD, mnpCrc(configRequestHeader,
    MNP_BINARY_HEADER_SIZE - 1)};
  uint16_t configRequestData[MNP_MODE_ID_CONFIG_WRITE_CMD_DATA_SIZE + 1] = //+1 CRC16
  {static_cast<uint16_t>(MNP_MODE_ID_CONFIG_CMD_CODE | source | MNP_BINARY_CMD_WRITE),
    RESERVED_WORD};
  for (uint8_t i = 0; i < MNP_CONFIG_WORDS_NUMBER; i++)
  {
    configRequestData[i * 2 + MNP_MODE_ID_CONFIG_CMD_DATA_SIZE] = *(configurationData + i) & 0xFFFF; //((*(configurationData + i) & 0xFF))
    // | ((*(configurationData + i) & 0xFF00));
    configRequestData[i * 2 + MNP_MODE_ID_CONFIG_CMD_DATA_SIZE + 1] = (*(configurationData + i) & 0xFFFF0000) >> 16; // (((*(configurationData + i) & 0xFF0000) >> 16) << 8)
    // | ((*(configurationData + i) & 0xFF000000) >> 24);
  }
  configRequestData[MNP_CONFIG_WORDS_NUMBER * 2 + MNP_MODE_ID_CONFIG_CMD_DATA_SIZE] = mnpCrc(configRequestData,
  MNP_CONFIG_WORDS_NUMBER * 2 + MNP_MODE_ID_CONFIG_CMD_DATA_SIZE);
  mnpBinaryOutLittleEndian(configRequestHeader, MNP_BINARY_HEADER_SIZE);
  mnpBinaryOutLittleEndian(configRequestData, MNP_MODE_ID_CONFIG_WRITE_CMD_DATA_SIZE + 1);
  clearRingBuffer(getGnssRxRingBuffer());
}

void gnssConfigurationSetting(GnssConfigStruct *initialStruct, uint32_t *configurationData)
{
  uint32_t channel0CfgWord = *(configurationData + CFG_CMD_CH0_SHIFT);
  uint32_t channel1CfgWord = *(configurationData + CFG_CMD_CH1_SHIFT);
  uint32_t cfgWord4 = *(configurationData + 4);
  uint32_t cfgWord5 = *(configurationData + 5);

  cfgWord5 &= GGA_MSG_RESET_MASK & GSA_MSG_RESET_MASK & GSV_MSG_RESET_MASK & RMC_MSG_RESET_MASK & CH1_MSG_RESET_MASK;
  cfgWord4 &= GLO_TIME_RESET_MASK & SYS_TIME_RESET_MASK;

  if (initialStruct->GLO_TIME)
  {
    cfgWord4 |= GLO_TIME_SET_MASK;
  }
  if (initialStruct->SYS_TIME)
  {
    cfgWord4 |= SYS_TIME_SET_MASK;
  }

  if (initialStruct->CH0_GGA_ENABLE)
  {
    cfgWord5 |= GGA_MSG_SET_MASK;
  }
  if (initialStruct->CH0_RMC_ENABLE)
  {
    cfgWord5 |= RMC_MSG_SET_MASK;
  }
  if (initialStruct->CH0_GSV_ENABLE)
  {
    cfgWord5 |= GSV_MSG_SET_MASK;
  }
  if (initialStruct->CH0_GSA_ENABLE)
  {
    cfgWord5 |= GSA_MSG_SET_MASK;
  }

  channel0CfgWord &= CFG_CMD_PROTOCOL_CLEAR_MASK & CFG_CMD_DIV_CLEAR_MASK;
  channel1CfgWord &= CFG_CMD_PROTOCOL_CLEAR_MASK & CFG_CMD_DIV_CLEAR_MASK;
  uint32_t div0 = CHANNEL_BOUDRATE_CONST / initialStruct->uart0Boudrate;
  uint32_t div1 = CHANNEL_BOUDRATE_CONST / initialStruct->uart1Boudrate;
  channel0CfgWord |= div0 << CFG_CMD_CHANNEL_DIV_SHIFT;
  channel1CfgWord |= div1 << CFG_CMD_CHANNEL_DIV_SHIFT;
  assert_parameter(IS_PROTOCOL(initialStruct->uart0Protocol));
  assert_parameter(IS_PROTOCOL(initialStruct->uart1Protocol));
  uint32_t protocol = CFG_CMD_CHANNEL_PROTOCOL_MNP_BINARY;
  if (initialStruct->uart0Protocol == NMEA)
  {
    protocol = CFG_CMD_CHANNEL_PROTOCOL_NMEA;
  }
  channel0CfgWord |= protocol;
  protocol = CFG_CMD_CHANNEL_PROTOCOL_MNP_BINARY;
  if (initialStruct->uart1Protocol == NMEA)
  {
    protocol = CFG_CMD_CHANNEL_PROTOCOL_NMEA;
  }
  channel1CfgWord |= protocol;

  *(configurationData + 4) = cfgWord4;
  *(configurationData + 5) = cfgWord5;
  *(configurationData + CFG_CMD_CH0_SHIFT) = channel0CfgWord;
  *(configurationData + CFG_CMD_CH1_SHIFT) = channel1CfgWord;

  mnpSendWriteConfigurationRequest(initialStruct->source, configurationData);
}

/*Sending read configuration  request
 *
 * source:   CMD_RAM
 *           CMD_FLASH
 * mode:      CMD_WRITE
 *           CMD_READ
 *
 * */
void mnpSendReadConfigurationRequest(uint16_t source)
{
  assert_parameter(IS_CONFIGURATION_REQUEST_SOURCE(source));
  uint16_t configRequestHeader[MNP_BINARY_HEADER_SIZE] =
  {SYNC_WORD,
    MNP_MODE_ID,
    MNP_MODE_ID_CONFIG_CMD_DATA_SIZE,
    RESERVED_WORD,
    mnpCrc(configRequestHeader, MNP_BINARY_HEADER_SIZE - 1),
  };
  uint16_t configRequestData[MNP_MODE_ID_CONFIG_CMD_DATA_SIZE + 1] =
  {static_cast<uint16_t>(MNP_MODE_ID_CONFIG_CMD_CODE | source | MNP_BINARY_CMD_READ),
    RESERVED_WORD,
    mnpCrc(configRequestData, MNP_MODE_ID_CONFIG_CMD_DATA_SIZE),
  };
  mnpBinaryOutLittleEndian(configRequestHeader, MNP_BINARY_HEADER_SIZE);
  mnpBinaryOutLittleEndian(configRequestData, MNP_MODE_ID_CONFIG_CMD_DATA_SIZE + 1);
  clearRingBuffer(getGnssRxRingBuffer());
}

int8_t mnpReadConfigurationData(uint32_t *data)
{
  if (getRingBufferCount(&mnpBinaryRxBuffer) < CONFIG_CMD_LEN)
    return 0;
  uint16_t word = showSymbolFromRingBuffer(0, &mnpBinaryRxBuffer);
  word |= ((uint16_t)showSymbolFromRingBuffer(1, &mnpBinaryRxBuffer)) << 8;
  if (word != SYNC_WORD)
    return 0;
  uint16_t headerCrc = showSymbolFromRingBuffer(MNP_HEADER_CRC_SHIFT, &mnpBinaryRxBuffer);
  headerCrc |= ((uint16_t)showSymbolFromRingBuffer(MNP_HEADER_CRC_SHIFT + 1, &mnpBinaryRxBuffer)) << 8;
  if (mnpCrcFromRingBuffer(&mnpBinaryRxBuffer, MNP_HEADER_SIZE - 1) == headerCrc)
  {
    uint16_t modeId = showSymbolFromRingBuffer(MNP_ID_SHIFT, &mnpBinaryRxBuffer);
    modeId |= ((uint16_t)showSymbolFromRingBuffer(MNP_ID_SHIFT + 1, &mnpBinaryRxBuffer)) << 8;
    uint16_t dataSize = showSymbolFromRingBuffer(MNP_DATA_SIZE_SHIFT, &mnpBinaryRxBuffer);
    dataSize |= ((uint16_t)showSymbolFromRingBuffer(MNP_DATA_SIZE_SHIFT + 1, &mnpBinaryRxBuffer)) << 8;
    for (uint8_t i = 0; i < MNP_HEADER_SIZE * 2; i++)
    {
      getSymbolFromRingBuffer(&mnpBinaryRxBuffer);
    }
    if (modeId == MNP_MODE_ID)
    {
      uint16_t dataCrc = showSymbolFromRingBuffer(dataSize * 2, &mnpBinaryRxBuffer);
      dataCrc |= ((uint16_t)showSymbolFromRingBuffer(dataSize * 2 + 1, &mnpBinaryRxBuffer)) << 8;
      const uint16_t crc16 = mnpCrcFromRingBuffer(&mnpBinaryRxBuffer, dataSize);
      if (crc16 == dataCrc)
      {
        for (uint8_t i = 0; i < MNP_MODE_ID_CONFIG_CMD_DATA_SIZE * 2; i++) //read cmd+reserved word
        {
          getSymbolFromRingBuffer(&mnpBinaryRxBuffer);
        }
        for (uint8_t i = 0; i < MNP_CONFIG_WORDS_NUMBER; i++)
        {
          uint32_t configWord;
          configWord = getSymbolFromRingBuffer(&mnpBinaryRxBuffer);
          configWord |= ((uint32_t)getSymbolFromRingBuffer(&mnpBinaryRxBuffer)) << 8;
          configWord |= ((uint32_t)getSymbolFromRingBuffer(&mnpBinaryRxBuffer)) << 16;
          configWord |= ((uint32_t)getSymbolFromRingBuffer(&mnpBinaryRxBuffer)) << 24;
          *(data + i) = configWord;
        }
        clearRingBuffer(&mnpBinaryRxBuffer);
        return 1;
      }
      else
      {
        getSymbolFromRingBuffer(&mnpBinaryRxBuffer);
      }
    }
    else
    {
      for (uint8_t i = 0; i < dataSize; i++)
      {
        getSymbolFromRingBuffer(&mnpBinaryRxBuffer);
      }
    }
  } //if (mnpCrcFromRingBuffer(&mnpBinaryRxBuffer, MNP_HEADER_SIZE - 1) == headerCrc)
  else
  {
    getSymbolFromRingBuffer(&mnpBinaryRxBuffer);
  }

  return 0;
}
