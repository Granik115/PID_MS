/*
 * pwm_timer_interface.h
 *
 *  Created on: Dec 3, 2024
 *      Author: mvm
 */

#ifndef THEODOLITE_LOGIC_FOC_PWM_TIMER_INTERFACE_H_
#define THEODOLITE_LOGIC_FOC_PWM_TIMER_INTERFACE_H_

#include "FOCDriver.h"

class PwmTimerInterface
{
public:
  virtual ~PwmTimerInterface()
  {

  }
  virtual void writeDutyCycle(float dc_a, float dc_b, float dc_c, PhaseState* phase_state)=0;
  virtual void enable()=0;
  virtual void disable()=0;
};


#endif /* THEODOLITE_LOGIC_FOC_PWM_TIMER_INTERFACE_H_ */
