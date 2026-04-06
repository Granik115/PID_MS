/*(c) 2016 NIC SPbETU
 * local_clock.cpp
 *
 *  Created on: 14 февр. 2017 г.
 *      Author: mmalenko
 */

#include "theodolite/logic/soft_timer/sw_timer.h"

static int32_t timerCount;

__attribute__((weak)) int32_t clock_time()
{
  return timerCount;
}

void localClockTimerISR()
{
  timerCount++;
}

