/*(c) 2016 NIC SPbETU
 * local_clock.h
 *
 *  Created on: 14 февр. 2017 г.
 *      Author: mmalenko
 */

#ifndef SOURCE_SW_TIMER_LOCAL_CLOCK_H_
#define SOURCE_SW_TIMER_LOCAL_CLOCK_H_

#include "stdint.h"

int32_t clock_time();
void localClockTimerConfig();
void localClockTimerISR();

#endif /* SOURCE_SW_TIMER_LOCAL_CLOCK_H_ */
