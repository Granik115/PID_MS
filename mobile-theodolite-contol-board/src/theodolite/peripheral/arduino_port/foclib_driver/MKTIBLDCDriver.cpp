#include <foclib_driver/MKTIBLDCDriver.h>

MKTIBLDCDriver::MKTIBLDCDriver(PwmTimerInterface &pwm_timer): m_pwmTimer(pwm_timer)
{
  // default power-supply value
  voltage_power_supply = DEF_POWER_SUPPLY;
  voltage_limit = NOT_SET;
  pwm_frequency = NOT_SET;

  // dead zone initial - 2%
  dead_zone = 0.02f;

}

// enable motor driver
void  MKTIBLDCDriver::enable(){
    // enable_pin the driver - if enablePin pin available
    if ( _isset(enable_pin) ) digitalWrite(enable_pin, enable_active_high);
    // set phase state enabled
    setPhaseState(PhaseState::PHASE_ON, PhaseState::PHASE_ON, PhaseState::PHASE_ON);
    // set zero to PWM
    setPwm(0, 0, 0);
    m_pwmTimer.enable();
}

// disable motor driver
void MKTIBLDCDriver::disable()
{
  // set phase state to disabled
  setPhaseState(PhaseState::PHASE_OFF, PhaseState::PHASE_OFF, PhaseState::PHASE_OFF);
  // set zero to PWM
  setPwm(0, 0, 0);
  // disable the driver - if enablePin pin available
  if ( _isset(enable_pin) ) digitalWrite(enable_pin, !enable_active_high);

  m_pwmTimer.disable();

}

// init hardware pins
int MKTIBLDCDriver::init() {

  // sanity check for the voltage limit configuration
  if( !_isset(voltage_limit) || voltage_limit > voltage_power_supply) voltage_limit =  voltage_power_supply;

  // set phase state to disabled
  phase_state[0] = PhaseState::PHASE_OFF;
  phase_state[1] = PhaseState::PHASE_OFF;
  phase_state[2] = PhaseState::PHASE_OFF;
  // set zero to PWM
  dc_a = dc_b = dc_c = 0;

  initialized = true;
  return 0;
}


// Set voltage to the pwm pin
void MKTIBLDCDriver::setPwm(float Ua, float Ub, float Uc) {
  // limit the voltage in driver
  Ua = _constrain(Ua, 0, voltage_limit);
  Ub = _constrain(Ub, 0, voltage_limit);
  Uc = _constrain(Uc, 0, voltage_limit);
  // calculate duty cycle
  // limited in [0,1]
  dc_a = _constrain(Ua / voltage_power_supply, 0.0f , 1.0f );
  dc_b = _constrain(Ub / voltage_power_supply, 0.0f , 1.0f );
  dc_c = _constrain(Uc / voltage_power_supply, 0.0f , 1.0f );
  // hardware specific writing
  // hardware specific function - depending on driver and mcu
  m_pwmTimer.writeDutyCycle(dc_a, dc_b, dc_c, phase_state);
}


// Set the phase state
// actually changing the state is only done on the next call to setPwm, and depends
// on the hardware capabilities of the driver and MCU.
void MKTIBLDCDriver::setPhaseState(PhaseState sa, PhaseState sb, PhaseState sc) {
  phase_state[0] = sa;
  phase_state[1] = sb;
  phase_state[2] = sc;
}
