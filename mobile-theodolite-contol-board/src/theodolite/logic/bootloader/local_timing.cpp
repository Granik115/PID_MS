///*
// * pwm.cpp
// *
// *      Author: mmalenko
// * (c) 2017 NIC SPbETU
// */

#include "stddef.h"
#include <theodolite/logic/axis_mover/axis_mover_interface.h>
#include <theodolite/logic/local_clock/local_clock.h>

void periodicIsrCall(void);

void periodicIsrCall(void)
{
  localClockTimerISR();
}
