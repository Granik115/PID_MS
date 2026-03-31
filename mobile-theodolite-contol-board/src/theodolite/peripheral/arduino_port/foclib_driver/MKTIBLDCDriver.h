#ifndef BLDCDriver6PWM_h
#define BLDCDriver6PWM_h

#include "../common/base_classes/BLDCDriver.h"
#include "../common/foc_utils.h"
#include "../common/time_utils.h"
#include "../common/defaults.h"
#include "foc/pwm_timer_interface.h"

/**
 6 pwm bldc driver class
 */
class MKTIBLDCDriver:
  public BLDCDriver
{
public:

  MKTIBLDCDriver(PwmTimerInterface&);

  /**  Motor hardware init function */
  int init() override;
  /** Motor disable function */
  void disable() override;
  /** Motor enable function */
  void enable() override;

  // hardware variables
  int enable_pin; //!< enable pin number

  float dead_zone; //!< a percentage of dead-time(zone) (both high and low side in low) for each pwm cycle [0,1]

  PhaseState phase_state[3]; //!< phase state (active / disabled)

  /**
   * Set phase voltages to the harware
   *
   * @param Ua - phase A voltage
   * @param Ub - phase B voltage
   * @param Uc - phase C voltage
   */
  void setPwm(float Ua, float Ub, float Uc) override;

  /**
   * Set phase voltages to the harware
   *
   * @param sc - phase A state : active / disabled ( high impedance )
   * @param sb - phase B state : active / disabled ( high impedance )
   * @param sa - phase C state : active / disabled ( high impedance )
   */
  virtual void setPhaseState(PhaseState sa, PhaseState sb, PhaseState sc) override;

private:

  PwmTimerInterface &m_pwmTimer;

};

#define _PWM_RANGE 4095.0f // 2^12 -1 = 4095

class AzimuthMotorPwmTimer:
  public PwmTimerInterface
{
public:
  void writeDutyCycle(float dc_a, float dc_b, float dc_c, PhaseState *phase_state)
  {
    if (phase_state[0] == PhaseState::PHASE_OFF)
      dc_a = 0.0f;
    LL_TIM_OC_SetCompareCH1(AZ_TIM, _PWM_RANGE * dc_a);
    if (phase_state[1] == PhaseState::PHASE_OFF)
      dc_b = 0.0f;
    LL_TIM_OC_SetCompareCH2(AZ_TIM, _PWM_RANGE * dc_b);
    if (phase_state[2] == PhaseState::PHASE_OFF)
      dc_c = 0.0f;
    LL_TIM_OC_SetCompareCH3(AZ_TIM, _PWM_RANGE * dc_c);
  }

  void enable()
  {
    LL_TIM_EnableAllOutputs(AZ_TIM);
    LL_GPIO_ResetOutputPin(AZ_PWM_BUS_ENABLE_GPIO_Port, AZ_PWM_BUS_ENABLE_Pin);
  }

  void disable()
  {
    LL_TIM_DisableAllOutputs(AZ_TIM);
    LL_GPIO_SetOutputPin(AZ_PWM_BUS_ENABLE_GPIO_Port, AZ_PWM_BUS_ENABLE_Pin);
  }
};

class ElevationMotorPwmTimer:
  public PwmTimerInterface
{
public:
  void writeDutyCycle(float dc_a, float dc_b, float dc_c, PhaseState *phase_state)
  {
    if (phase_state[0] == PhaseState::PHASE_OFF)
      dc_a = 0.0f;
    LL_TIM_OC_SetCompareCH1(EL_TIM, _PWM_RANGE * dc_a);
    if (phase_state[1] == PhaseState::PHASE_OFF)
      dc_b = 0.0f;
    LL_TIM_OC_SetCompareCH2(EL_TIM, _PWM_RANGE * dc_b);
    if (phase_state[2] == PhaseState::PHASE_OFF)
      dc_c = 0.0f;
    LL_TIM_OC_SetCompareCH3(EL_TIM, _PWM_RANGE * dc_c);
  }

  void enable()
  {
    LL_TIM_EnableAllOutputs(EL_TIM);
    LL_GPIO_ResetOutputPin(UM_PWM_BUS_ENABLE_GPIO_Port, UM_PWM_BUS_ENABLE_Pin);
  }

  void disable()
  {
    LL_TIM_DisableAllOutputs(EL_TIM);
    LL_GPIO_SetOutputPin(UM_PWM_BUS_ENABLE_GPIO_Port, UM_PWM_BUS_ENABLE_Pin);
  }
};

#endif
