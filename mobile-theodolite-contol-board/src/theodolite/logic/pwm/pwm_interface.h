/*
 * pwm_interface.h
 *
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#ifndef LOGIC_PWM_PWM_INTERFACE_H_
#define LOGIC_PWM_PWM_INTERFACE_H_

#include "stdint.h"
#include <theodolite/logic/ramp_control/ramp_control_interface.h>

typedef enum
{
  INCREMENT_ANGLE_DIRECTION,
  DECREMENT_ANGLE_DIRECTION
} RotationDirection;

class PwmInterface
{
public:
  virtual void enableInertion()=0;
  virtual void disableInertion()=0;
  virtual void setLinearInertionTimeCoeff(float coeff)=0;
  virtual void setDuty(float percent)=0;
  virtual void setRampMode(RampMode)=0;
  virtual float getDuty()=0;
  virtual float getModelDuty()=0;
  virtual int16_t getTimerDuty()=0;
  virtual void updateOnRequest()=0;
  virtual void updateWithoutRequest()=0;
  virtual void setUpdateRequest()=0;
  virtual ~PwmInterface()
  {

  }
};

#endif /* LOGIC_PWM_PWM_INTERFACE_H_ */
