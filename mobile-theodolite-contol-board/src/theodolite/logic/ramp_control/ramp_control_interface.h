/*
 * ramp_control_interface.h
 *
 *  Created on: 4 авг. 2017 г.
 *      Author: mmalenko
 *   (c) 2017 NIC SPbETU
 */

#ifndef SRC_THEODOLITE_LOGIC_RAMP_CONTROL_RAMP_CONTROL_INTERFACE_H_
#define SRC_THEODOLITE_LOGIC_RAMP_CONTROL_RAMP_CONTROL_INTERFACE_H_

typedef enum
{
  RAMPCONTROL_LINEAR,
  RAMPCONTROL_EXPONENT
} RampMode;

class RampControlInterface
{
public:
  virtual void setDelta(float)=0;
  virtual void setTargetValue(float)=0;
  virtual bool isEqual()=0;
  virtual float getSetPointValue()=0;
  virtual void enable()=0;
  virtual void disable()=0;
  virtual float calculateNextSetPoint()=0;
  virtual float calculateNextSetPoint(float reference)=0;
  virtual void setMode(RampMode)=0;
  virtual ~RampControlInterface()
  {

  }
};

#endif /* SRC_THEODOLITE_LOGIC_RAMP_CONTROL_RAMP_CONTROL_INTERFACE_H_ */
