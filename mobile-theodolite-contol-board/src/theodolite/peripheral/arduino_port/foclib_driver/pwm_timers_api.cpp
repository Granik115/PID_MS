/*
 * pwm_timers_api.c
 *
 *  Created on: Dec 3, 2024
 *      Author: mvm
 */
#include "main.h"
#include "foc/pwm_timer_interface.h"



class ElevationMotorPwmTimer:
  public PwmTimerInterface
{
  virtual ~ElevationMotorPwmTimer()
  {

  }
  virtual void writeDutyCycle(float dc_a, float dc_b, float dc_c, PhaseState *phase_state)
  {

  }
};

