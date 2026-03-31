#ifndef LOWSIDE_CS_LIB_H
#define LOWSIDE_CS_LIB_H

#include "Arduino.h"
#include "../common/foc_utils.h"
#include "../common/time_utils.h"
#include "../common/defaults.h"
#include "../common/base_classes/CurrentSense.h"
#include "../common/base_classes/FOCMotor.h"
#include "../common/base_classes/StepperDriver.h"
#include "../common/base_classes/BLDCDriver.h"
#include "../common/lowpass_filter.h"
#include "hardware_api.h"
#include "foc/current_sensor_adc_interface.h"

class MKTILowsideCurrentSense:
  public CurrentSense
{
public:
  /**
   LowsideCurrentSense class constructor
   @param shunt_resistor shunt resistor value
   @param gain current-sense op-amp gain
   @param phA A phase adc pin
   @param phB B phase adc pin
   @param phC C phase adc pin (optional)
   */
  MKTILowsideCurrentSense(float shunt_resistor, float gain, AdcInterface &adc);
  /**
   LowsideCurrentSense class constructor
   @param mVpA mV per Amp ratio
   @param phA A phase adc pin
   @param phB B phase adc pin
   @param phC C phase adc pin (optional)
   */
  MKTILowsideCurrentSense(float mVpA, AdcInterface &adc);

  // CurrentSense interface implementing functions
  int init() override;
  PhaseCurrent_s getPhaseCurrents() override;

private:

  // gain variables
  float shunt_resistor; //!< Shunt resistor value
  float amp_gain; //!< amp gain value
  float volts_to_amps_ratio; //!< Volts to amps ratio
  AdcInterface &m_adc;
  /**
   *  Function finding zero offsets of the ADC
   */
  void calibrateOffsets();

};

#endif
