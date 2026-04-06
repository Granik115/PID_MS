/*(c) 2016 NIC SPbETU
 * sw_timer.cpp
 *
 *  Created on: 14 февр. 2017 г.
 *      Author: mmalenko
 */
#include <theodolite/logic/local_clock/local_clock.h>
#include "theodolite/logic/soft_timer/sw_timer.h"
#include "stdlib.h"

void Timer::set(int32_t interval)
{
  this->interval = interval;
  begin = clock_time();
}

void Timer::start()
{
  if (!running)
  {
    restart();
    running = true;
  }
}

void Timer::stop()
{
  running = false;
}

bool Timer::isRunning()
{
  return running;
}

void Timer::reset()
{
  begin += interval;
}

void Timer::restart()
{
  begin = clock_time();
}

Timer::Timer(int32_t interval)
{
  this->running = false;
  this->begin = clock_time();
  this->interval = interval;
}

bool Timer::expired()
{
  return ((abs(clock_time() - begin)) >= interval);
}

int32_t Timer::remaining()
{
  return begin + interval - clock_time();
}
