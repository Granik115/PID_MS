/*
 * current_sensor_adc.h
 *
 *  Created on: Dec 18, 2024
 *      Author: mvm
 */

#ifndef THEODOLITE_PERIPHERAL_ARDUINO_PORT_FOCLIB_CURRENT_SENSE_CURRENT_SENSOR_ADC_H_
#define THEODOLITE_PERIPHERAL_ARDUINO_PORT_FOCLIB_CURRENT_SENSE_CURRENT_SENSOR_ADC_H_

#include "foc/current_sensor_adc_interface.h"
#include "foc/adc_hardware_api.h"

#define ADC_K ((float)3.3/4096)

class AzimuthMotorCurrentAdc:
  public AdcInterface
{
public:
  virtual ~AzimuthMotorCurrentAdc()
  {

  }

  float getPhaseAVoltage()
  {
    return getAzimuthAdcMeasure(MotorAdcMeasure_A) * ADC_K;
  }

  float getPhaseBVoltage()
  {
    return getAzimuthAdcMeasure(MotorAdcMeasure_B) * ADC_K;
  }

  float getPhaseCVoltage()
  {
    return getAzimuthAdcMeasure(MotorAdcMeasure_C) * ADC_K;
  }

  float getBreakVoltage()
  {
    return getAzimuthAdcMeasure(MotorAdcMeasure_BREAK) * ADC_K;
  }

  float getTemperatureVoltage()
  {
    return getAzimuthAdcMeasure(MotorAdcMeasure_TEMPERATURE) * ADC_K;
  }

  bool sync(PwmTimerInterface *timer)
  {
    _UNUSED(timer);
    return true;
  }
};

class ElevationMotorCurrentAdc:
  public AdcInterface
{
public:
  virtual ~ElevationMotorCurrentAdc()
  {

  }

  float getPhaseAVoltage()
  {
    return getElevationAdcMeasure(MotorAdcMeasure_A) * ADC_K;
  }

  float getPhaseBVoltage()
  {
    return getElevationAdcMeasure(MotorAdcMeasure_B) * ADC_K;
  }

  float getPhaseCVoltage()
  {
    return getElevationAdcMeasure(MotorAdcMeasure_C) * ADC_K;
  }

  float getBreakVoltage()
  {
    return getElevationAdcMeasure(MotorAdcMeasure_BREAK) * ADC_K;
  }

  float getTemperatureVoltage()
  {
    return getElevationAdcMeasure(MotorAdcMeasure_TEMPERATURE) * ADC_K;
  }

  bool sync(PwmTimerInterface *timer)
  {
    _UNUSED(timer);
    return true;
  }
};

#endif /* THEODOLITE_PERIPHERAL_ARDUINO_PORT_FOCLIB_CURRENT_SENSE_CURRENT_SENSOR_ADC_H_ */
