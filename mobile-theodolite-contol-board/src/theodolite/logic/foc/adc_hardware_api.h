/*
 * adc_hardware_api.h
 *
 *  Created on: Dec 24, 2024
 *      Author: mvm
 */

#ifndef THEODOLITE_LOGIC_FOC_ADC_HARDWARE_API_H_
#define THEODOLITE_LOGIC_FOC_ADC_HARDWARE_API_H_

#ifdef  __cplusplus
extern "C"
{
#endif

typedef enum
{
  MotorAdcMeasure_A = 0,
  MotorAdcMeasure_B,
  MotorAdcMeasure_C,
  MotorAdcMeasure_BREAK,
  MotorAdcMeasure_TEMPERATURE,
} MotorAdcMeasure;

#define A 8

uint16_t getAzimuthAdcMeasure(MotorAdcMeasure);
uint16_t getElevationAdcMeasure(MotorAdcMeasure);

#ifdef  __cplusplus
}
#endif

#endif /* THEODOLITE_LOGIC_FOC_ADC_HARDWARE_API_H_ */
