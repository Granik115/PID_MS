/*
 * pwm_test.h
 *
 *  Created on: 7 авг. 2017 г.
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#ifndef PWM_TEST_H_
#define PWM_TEST_H_

#include <theodolite/logic/pwm/motor_pwm.h>
#include <theodolite/logic/sys_config/sys_cfg.h>

class PwmGeneratorTest:
  public MotorPwm
{
  void setPeripheralIncrementDirection()
  {

  }
  void setPeripheralDecrementDirection()
  {

  }

  void updatePeripheralSetting()
  {

  }
public:
  PwmGeneratorTest(int16_t periodMax) :
    MotorPwm(periodMax)
  {

  }
};

#endif /* PWM_TEST_H_ */
