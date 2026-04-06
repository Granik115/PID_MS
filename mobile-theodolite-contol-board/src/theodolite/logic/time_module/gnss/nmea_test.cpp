/*
 * nmea_test.c
 *
 *  Created on: 13 апр. 2016 г.
 *      Author: mmalenko
 *  (c) 2015 NIC SPbETU
 */

#ifdef  _TIME_BLOCK_TEST

#include "gtest/gtest.h"
#include "mnp_binary.h"
#include "gnss.h"
#include "time.h"

char test_vector[] =
{ "$GNRMC,024144.800,A,5957.7466,N,03018.0464,E,0.83,356.65,260314,,,A*74\r\n"
  "$GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68\r\n" /* good */
};

extern "C"
{
  extern void nmeaParserInit();
  extern int32_t nmeaParser();
}

TEST(GnssTest, ParseNmeaRMC)
{
  int32_t status = 0;
  for (uint8_t i = 0; i < sizeof(test_vector); i++)
  {
    putSymbolToRingBuffer(test_vector[i], getGnssRxRingBuffer());
  }
  nmeaParserInit();
  for (uint16_t i = 0; i < sizeof(test_vector); i++)
  {
    status = nmeaParser();
    if (status != 0)
    {
      break;
    }
  }
  ASSERT_NE(0, status);
}

extern "C"
{
  extern void convertRmcTime(nmeap_rmc_t *rmc, struct tm *time);
}

TEST(GnssTest, ConvertRmcTime)
{
  nmeap_rmc_t rmc;
  rmc.date = 140312;
  rmc.time = 23413;
  struct tm timeS;
  convertRmcTime(&rmc, &timeS);
  ASSERT_EQ(timeS.tm_hour, 2);
  ASSERT_EQ(timeS.tm_min, 34);
  ASSERT_EQ(timeS.tm_sec, 13);
  ASSERT_EQ(timeS.tm_year, 2012);
  ASSERT_EQ(timeS.tm_mon, 3);
  ASSERT_EQ(timeS.tm_mday, 14);
}

#endif
