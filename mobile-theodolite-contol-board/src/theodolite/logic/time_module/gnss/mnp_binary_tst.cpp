/*
 * mnp_binary_tst.cpp
 *
 *  Created on: 8 апр. 2016 г.
 *      Author: mmalenko
 *  (c) 2015 NIC SPbETU
 */

#ifdef  _TIME_BLOCK_TEST

#include "gtest/gtest.h"
#include "mnp_binary.h"

uint8_t testModeData[] =
{ 0xFF, 0x81, 0xBE, 0x0B, 0x02, 0x00, 0x00, 0x00, 0x41, 0x72 //Header
  , 0x01, 0x2, 0x00, 0x00,// Cmd code
  0xFF, 0xFD//CRC DATA
};

uint8_t testModeReplyData[] =
{ 0xFF, 0x81, 0xBE, 0x0B, 0x0E, 0x00, 0x00, 0x00, 0x35, 0x72 //Header
  , 0x1, 0x2, 0x00, 0x00,// Cmd code
  0x39, 0x14, 0x00, 0x04,//0
  0x08, 0x00, 0x00, 0x0C,//1
  0x01, 0x00, 0x00, 0x01,//2
  0x00, 0x00, 0xA6, 0x00,//3
  0x04, 0xF7, 0x04, 0xFF,//4
  0x01, 0x07, 0x00, 0x00,//5
  0x0E, 0xDB//CRC
};

uint8_t testModeWriteData[] =
{ 0xFF, 0x81, 0xBE, 0x0B, 0x0E, 0x00, 0x00, 0x00, 0x35, 0x72 //Header
  , 0x81, 0x2, 0x00, 0x00,// Cmd code
  0x39, 0x14, 0x00, 0x04,//0
  0x08, 0x00, 0x00, 0x0C,//1
  0x01, 0x00, 0x00, 0x01,//2
  0x00, 0x00, 0xA6, 0x00,//3
  0x04, 0xF7, 0x04, 0xFF,//4
  0x01, 0x07, 0x00, 0x00,//5
  0x8E, 0xDA//CRC
};

uint8_t testGnssConfigFuncData[] =
{ 0xFF, 0x81, 0xBE, 0x0B, 0x0E, 0x00, 0x00, 0x00, 0x35, 0x72 //Header
  , 0x81, 0x2, 0x00, 0x00,// Cmd code
  0x39, 0x14, 0x00, 0x04,//0
  0x08, 0x00, 0x00, 0x01,//1   //1000008
  0x01, 0x00, 0x00, 0x01,//2
  0x00, 0x00, 0xA6, 0x00,//3
  0x04, 0xF7, 0x06, 0xFF,//4
  0x08, 0x00, 0x00, 0x00,//5
  0x85, 0xEC//CRC
};

uint32_t gnssConfigWords[6] =
{
  0x4001439, 0xc000008, 0x1000001, 0xa60000, 0xff04f704, 0x701
};

TEST(MnpBinaryProtocol,RequestConfiguration)
{
  uint16_t summ = 0;
  for (uint8_t i = 0; i < 14; i++)
  {
    summ += (((uint16_t)(testGnssConfigFuncData[i * 2 + 10 + 1])) << 8)
    | (((uint16_t)(testGnssConfigFuncData[i * 2 + 10])));
  }

  mnpSendReadConfigurationRequest(MNP_BINARY_CMD_RAM);
  uint8_t count = getRingBufferCount(getGnssTxRingBuffer());
  ASSERT_EQ(count, sizeof(testModeData));
  for (uint8_t i = 0; i < count; i++)
  {
    ASSERT_EQ(testModeData[i], getSymbolFromRingBuffer(getGnssTxRingBuffer()));
  }
}

extern "C"
{
  extern void mnpSendWriteConfigurationRequest(uint16_t source, uint32_t *configurationData);
}

TEST(MnpBinaryProtocol,WriteConfiguration)
{
  uint32_t gnssConfigWordsTmp[sizeof(gnssConfigWords)];
  for (uint8_t i = 0; i < sizeof(gnssConfigWords); i++)
  {
    gnssConfigWordsTmp[i] = gnssConfigWords[i];
  }
  mnpSendWriteConfigurationRequest(MNP_BINARY_CMD_RAM, gnssConfigWordsTmp);
  uint8_t count = getRingBufferCount(getGnssTxRingBuffer());
  ASSERT_EQ(count, sizeof(testModeWriteData));
  for (uint8_t i = 0; i < count; i++)
  {
    ASSERT_EQ(testModeWriteData[i], getSymbolFromRingBuffer(getGnssTxRingBuffer()));
  }
}

TEST(MnpBinaryProtocol,ConfigurationSetting)
{
  GnssConfigStruct initialStruct;
  initialStruct.uart0Boudrate = 115200;
  initialStruct.uart1Boudrate = 115200;
  initialStruct.uart0Protocol = NMEA;
  initialStruct.uart1Protocol = MNP_BINARY;
  initialStruct.GLO_TIME = 1;
  initialStruct.SYS_TIME = 0;
  initialStruct.CH0_RMC_ENABLE = 1;
  initialStruct.source = MNP_BINARY_CMD_RAM;
  uint32_t gnssConfigWordsTmp[sizeof(gnssConfigWords)];
  for (uint8_t i = 0; i < sizeof(gnssConfigWords); i++)
  {
    gnssConfigWordsTmp[i] = gnssConfigWords[i];
  }
  gnssConfigurationSetting(&initialStruct, gnssConfigWordsTmp);
  uint8_t count = getRingBufferCount(getGnssTxRingBuffer());
  ASSERT_EQ(count, sizeof(testGnssConfigFuncData));
  for (uint8_t i = 0; i < count; i++)
  {
    ASSERT_EQ(testGnssConfigFuncData[i], getSymbolFromRingBuffer(getGnssTxRingBuffer()));
  }
}

TEST(MnpBinaryProtocol,ReadConfigWords)
{
  uint32_t configWords[6] =
  {};

  for (uint8_t i = 0; i < sizeof(testModeReplyData); i++)
  {
    putSymbolToRingBuffer(testModeReplyData[i], getGnssRxRingBuffer());
  }
  ASSERT_EQ(1, mnpReadConfigurationData(configWords));
  for (uint8_t i = 0; i < 6; i++)
  {
    uint32_t word = (uint32_t)testModeReplyData[i * 4 + 14];
    word |= ((uint32_t)testModeReplyData[i * 4 + 15]) << 8;
    word |= ((uint32_t)testModeReplyData[i * 4 + 16]) << 16;
    word |= ((uint32_t)testModeReplyData[i * 4 + 17]) << 24;
    ASSERT_EQ(configWords[i], word);
  }
}

#endif
