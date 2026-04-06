/*
 * ramp_control.h
 *
 *      Author: mmalenko
 *  (c) 2017 NIC SPbETU
 */

#ifndef LOGIC_RAMP_CONTROL_RAMP_CONTROL_H_
#define LOGIC_RAMP_CONTROL_RAMP_CONTROL_H_

#include "stdint.h"
#include <theodolite/logic/ramp_control/ramp_control_interface.h>

class RampControl:
  public RampControlInterface
{
protected:
  RampMode mode;
  float targetValue;
  float delta;
  float funcArg;
  float setpointValue;
  static const int EXPONENT_MUL = 5;
  bool rampEnabled;
  public:
  void enable();
  void disable();
  void setDelta(float);
  void setTargetValue(float);
  bool isEqual();
  RampControl(float, RampMode);
  float getSetPointValue();
  void setMode(RampMode);
  float calculateNextSetPoint();
  float calculateNextSetPoint(float);
};

#endif /* LOGIC_RAMP_CONTROL_RAMP_CONTROL_H_ */
