/*
 * adc_interface.h
 *
 *  Created on: Dec 17, 2024
 *      Author: mvm
 */

#ifndef THEODOLITE_LOGIC_FOC_CURRENT_SENSOR_ADC_INTERFACE_H_
#define THEODOLITE_LOGIC_FOC_CURRENT_SENSOR_ADC_INTERFACE_H_

#include "foc/pwm_timer_interface.h"

class AdcInterface
{
public:
  virtual ~AdcInterface()
  {

  }
  virtual float getPhaseAVoltage()=0;
  virtual float getPhaseBVoltage()=0;
  virtual float getPhaseCVoltage()=0;
  virtual float getBreakVoltage()=0;
  virtual float getTemperatureVoltage()=0;
  virtual bool sync(PwmTimerInterface*)=0;
};

#endif /* THEODOLITE_LOGIC_FOC_CURRENT_SENSOR_ADC_INTERFACE_H_ */
