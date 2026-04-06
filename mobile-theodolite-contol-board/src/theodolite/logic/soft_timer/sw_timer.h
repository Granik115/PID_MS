/*(c) 2016 NIC SPbETU
 * sw_timer.h
 *
 *  Created on: 14 февр. 2017 г.
 *      Author: mmalenko
 */

#ifndef SOURCE_SW_TIMER_SW_TIMER_H_
#define SOURCE_SW_TIMER_SW_TIMER_H_

#include "stdint.h"

class Timer
{
  int32_t begin;
  int32_t interval;
  bool running;
  public:
  Timer(int32_t interval);
  void set(int32_t interval);
  void reset();
  void start();
  void stop();
  bool isRunning();
  void restart();
  bool expired();
  int32_t remaining();
};

#endif /* SOURCE_SW_TIMER_SW_TIMER_H_ */
