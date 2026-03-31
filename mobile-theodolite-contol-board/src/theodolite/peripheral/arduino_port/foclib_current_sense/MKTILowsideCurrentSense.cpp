#include <foclib_current_sense/MKTILowsideCurrentSense.h>
#include "communication/SimpleFOCDebug.h"

// MKTILowsideCurrentSense constructor
//  - shunt_resistor  - shunt resistor value
//  - gain  - current-sense op-amp gain
MKTILowsideCurrentSense::MKTILowsideCurrentSense(float _shunt_resistor, float _gain, AdcInterface &adc) :
  m_adc(adc)
{
  shunt_resistor = _shunt_resistor;
  amp_gain = _gain;
  volts_to_amps_ratio = 1.0f / _shunt_resistor / _gain; // volts to amps
  // gains for each phase
  gain_a = volts_to_amps_ratio;
  gain_b = volts_to_amps_ratio;
  gain_c = volts_to_amps_ratio;
}

MKTILowsideCurrentSense::MKTILowsideCurrentSense(float _mVpA, AdcInterface &adc) :
  m_adc(adc)
{
  volts_to_amps_ratio = 1000.0f / _mVpA; // mV to amps
  // gains for each phase
  gain_a = volts_to_amps_ratio;
  gain_b = volts_to_amps_ratio;
  gain_c = volts_to_amps_ratio;
}

// Lowside sensor init function
int MKTILowsideCurrentSense::init()
{

  if (driver == nullptr)
  {
    SIMPLEFOC_DEBUG("CUR: Driver not linked!");
    return 0;
  }

  // sync the driver
  if (!m_adc.sync((PwmTimerInterface*)driver->params))
    return 0;
  // set the center pwm (0 voltage vector)
  if (driver_type == DriverType::BLDC)
    static_cast<BLDCDriver*>(driver)->setPwm(driver->voltage_limit / 2, driver->voltage_limit / 2,
                                             driver->voltage_limit / 2);
  // calibrate zero offsets
  calibrateOffsets();
  // set zero voltage to all phases
  if (driver_type == DriverType::BLDC)
    static_cast<BLDCDriver*>(driver)->setPwm(0, 0, 0);
  // set the initialized flag
  initialized = 1;
  // return success
  return 1;
}
// Function finding zero offsets of the ADC
void MKTILowsideCurrentSense::calibrateOffsets()
{
  const int calibration_rounds = 2000;

  // find adc offset = zero current voltage
  offset_ia = 0;
  offset_ib = 0;
  offset_ic = 0;
  // read the adc voltage 1000 times ( arbitrary number )
  for (int i = 0; i < calibration_rounds; i++)
  {
    offset_ia += m_adc.getPhaseAVoltage();
    offset_ib += m_adc.getPhaseBVoltage();
    offset_ic += m_adc.getPhaseCVoltage();
    _delay(1);
  }
  // calculate the mean offsets
  offset_ia = offset_ia / calibration_rounds;
  offset_ib = offset_ib / calibration_rounds;
  offset_ic = offset_ic / calibration_rounds;
}

// read all three phase currents (if possible 2 or 3)
PhaseCurrent_s MKTILowsideCurrentSense::getPhaseCurrents()
{
  PhaseCurrent_s current;
  current.a = (m_adc.getPhaseAVoltage() - offset_ia) * gain_a;    // amps
  current.b = (m_adc.getPhaseBVoltage() - offset_ib) * gain_b;    // amps
  current.c = (m_adc.getPhaseCVoltage() - offset_ic) * gain_c; // amps
  return current;
}
