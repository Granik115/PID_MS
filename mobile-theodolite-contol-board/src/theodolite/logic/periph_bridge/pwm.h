/* pwm.h
 *  Created on: 17 янв. 2017 г.
 *      Author: mmalenko
 *
 *  (c) 2017 NIC SPbETU
 */

#ifndef PERIPHERAL_PWM_PWM_H_
#define PERIPHERAL_PWM_PWM_H_

#include "../pwm/motor_pwm.h"
#include "stdlib.h"

class AzimuthMotor:
  public MotorPwm
{
  void setPeripheralIncrementDirection();
  void setPeripheralDecrementDirection();
  public:
  AzimuthMotor(int16_t periodMax) :
    MotorPwm(periodMax)
  {

  }
  virtual ~AzimuthMotor()
  {

  }
};

class ElevationMotor:
  public MotorPwm
{
  void setPeripheralIncrementDirection();
  void setPeripheralDecrementDirection();
  public:
  ElevationMotor(uint16_t periodMax) :
    MotorPwm(periodMax)
  {

  }
  virtual ~ElevationMotor()
  {

  }
};

void pwmConfig();

#endif /* PERIPHERAL_PWM_PWM_H_ */
