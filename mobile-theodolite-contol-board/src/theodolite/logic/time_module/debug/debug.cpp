/*
 * debug.c
 *
 *  Created on: Oct 27, 2023
 *      Author: mvm
 */
#include "time_module/gnss/gnss.h"
#include "debug/debug.h"
#include "stdio.h"

void debugTask()
{
  /*
  static int32_t lastClock;
  nmeap_rmc_t *rmc = getGnssRmcStruct();
  if (!rmc)
    return;

  if (clock_time() - lastClock < 1000)
    return;

  lastClock = clock_time();
  char buffer[100];
  const int32_t writeSize = snprintf(buffer, sizeof(buffer), "glonass time: %d\n", rmc->time);
  sendDataToEthernetApp(DEBUG_PORT, reinterpret_cast<uint8_t*>(&buffer[0]), writeSize);*/
}

