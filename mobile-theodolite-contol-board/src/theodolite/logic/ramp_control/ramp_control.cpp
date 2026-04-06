/*
 * ramp_control.cpp
 *
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#include "ramp_control.h"
#include "math.h"

static const double EXP_ARG_DELTA = 0.0015;

void RampControl::setTargetValue(float value)
{
  targetValue = value;
  if (rampEnabled == false)
  {
    setpointValue = targetValue;
  }
}

bool RampControl::isEqual()
{
  return targetValue == setpointValue;
}

RampControl::RampControl(float delta, RampMode mode)
{
  rampEnabled = true;
  this->mode = mode;
  this->delta = delta;
  targetValue = 0;
  setpointValue = 0;
  funcArg = 0;
}

void RampControl::enable()
{
  rampEnabled = true;
}
void RampControl::disable()
{
  rampEnabled = false;
  setTargetValue(targetValue);
}

void RampControl::setMode(RampMode mode)
{
  this->mode = mode;
}

void RampControl::setDelta(float delta)
{
  this->delta = delta;
}

float RampControl::getSetPointValue()
{
  return setpointValue;
}

float RampControl::calculateNextSetPoint(float reference)
{
  calculateNextSetPoint();
  return setpointValue;
}

float RampControl::calculateNextSetPoint()
{
  if (mode != RAMPCONTROL_EXPONENT)
  {
    funcArg = 0;
  }
  if (setpointValue > targetValue)
  {
    if (mode == RAMPCONTROL_LINEAR)
    {
      setpointValue -= delta;
    }
    else if (mode == RAMPCONTROL_EXPONENT) //Work only for manual controller
    {
      funcArg += EXP_ARG_DELTA;
      setpointValue = -1 * expf(funcArg) * EXPONENT_MUL;
    }
    if (setpointValue < targetValue)
    {
      funcArg = 0;
      setpointValue = targetValue;
    }
  }
  else if (setpointValue < targetValue)
  {
    if (mode == RAMPCONTROL_LINEAR)
    {
      setpointValue += delta;
    }
    else if (mode == RAMPCONTROL_EXPONENT)
    {
      funcArg += EXP_ARG_DELTA;
      setpointValue = expf(funcArg) * EXPONENT_MUL;
    }
    if (setpointValue > targetValue)
    {
      funcArg = 0;
      setpointValue = targetValue;
    }
  }
  return setpointValue;
}
