/*
 * pwm_generator.h
 *
 *  Created on: 14 июл. 2017 г.
 *      Author: mmalenko
 *  (c) 2017 NIC SPbETU
 */

#ifndef SRC_THEODOLITE_LOGIC_PWM_MOTOR_PWM_H_
#define SRC_THEODOLITE_LOGIC_PWM_MOTOR_PWM_H_

#include "theodolite/logic/pwm/pwm_interface.h"
#include "theodolite/logic/ramp_control/ramp_control.h"
#include <theodolite/logic/soft_timer/sw_timer.h>

class MotorPwmDummy:
  public PwmInterface
{
public:
  void enableInertion()
  {

  }
  void disableInertion()
  {

  }
  void setLinearInertionTimeCoeff(float coeff)
  {

  }
  void setDuty(float percent)
  {

  }
  float getDuty()
  {
    return 0;
  }
  float getModelDuty()
  {
    return 0;
  }
  void setRampMode(RampMode)
  {

  }
  void setUpdateRequest()
  {

  }
  void updateOnRequest()
  {

  }
  void updateWithoutRequest()
  {

  }
  int16_t getTimerDuty()
  {
    return 0;
  }
  ~MotorPwmDummy()
  {

  }
  MotorPwmDummy()
  {

  }
};

class MotorPwm:
  public PwmInterface
{
  RampControl *rampControl;
  RampControl m_modelDuty;
  int16_t timerDuty;
  float dutyPersent;
  int16_t periodMax;
  bool updateRequest;
  void setDutyFunc(int16_t dutyFunc);
  void setPeriodMax(int16_t periodMax);
  void calculateNewDuty();
  void updatePeripheralSetting();
  protected:
  virtual void setPeripheralIncrementDirection()=0;
  virtual void setPeripheralDecrementDirection()=0;
  public:
  void enableInertion();
  void disableInertion();
  void setLinearInertionTimeCoeff(float coeff);
  void setDuty(float percent);
  float getDuty();
  float getModelDuty();
  void setRampMode(RampMode);
  void setUpdateRequest();
  void updateOnRequest();
  void updateWithoutRequest();
  int16_t getTimerDuty();
  ~MotorPwm();
  MotorPwm(int16_t periodMax);
};

#endif /* SRC_THEODOLITE_LOGIC_PWM_MOTOR_PWM_H_ */
