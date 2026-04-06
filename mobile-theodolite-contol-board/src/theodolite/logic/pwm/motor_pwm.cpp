/*
 * pwm_generator.cpp
 *
 *  Created on: 14 июл. 2017 г.
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */
#include <theodolite/logic/pwm/motor_pwm.h>
#include <theodolite/logic/sys_config/sys_cfg.h>

const float INTEGRATOR_MAX_OUT = 100; //100%
const float SYSTEM_UPDATE_PERIOD_S = 0.001; //1 kHz период с которым вызывается пересчет всех параметров
const float MODEL_TIME = 0.2;
const float MODEL_DELTA = (INTEGRATOR_MAX_OUT * SYSTEM_UPDATE_PERIOD_S) / MODEL_TIME;

void MotorPwm::setDutyFunc(int16_t dutyFunc)
{
  this->timerDuty = dutyFunc;
}

void MotorPwm::updateWithoutRequest()
{
  calculateNewDuty();
  m_modelDuty.calculateNextSetPoint();
  updatePeripheralSetting();
}

void MotorPwm::updateOnRequest()
{
  if (updateRequest)
  {
    updateRequest = false;
    updateWithoutRequest();
  }
}

void MotorPwm::setDuty(float percent)
{
  dutyPersent = percent;
  rampControl->setTargetValue(percent);
  m_modelDuty.setTargetValue(percent);
}

float MotorPwm::getDuty()
{
  //dutyPersent = (static_cast<float>(timerDuty) / static_cast<float>(periodMax)) * INTEGRATOR_MAX_OUT;
  return dutyPersent;
}

float MotorPwm::getModelDuty()
{
  return m_modelDuty.getSetPointValue();
}

void MotorPwm::enableInertion()
{
  rampControl->enable();
}

void MotorPwm::disableInertion()
{
  rampControl->disable();
}

void MotorPwm::setLinearInertionTimeCoeff(float time)
{
  if (time > 0)
  {
    const float delta = (INTEGRATOR_MAX_OUT * SYSTEM_UPDATE_PERIOD_S) / time;
    rampControl->setDelta(delta);
  }
}

MotorPwm::~MotorPwm()
{

}

MotorPwm::MotorPwm(int16_t periodMax) :
  m_modelDuty(MODEL_DELTA, RAMPCONTROL_LINEAR)
{
  updateRequest = false;
  timerDuty = 0;
  dutyPersent = 0;
  this->periodMax = periodMax;
  rampControl = new RampControl(0, RAMPCONTROL_LINEAR);
  rampControl->setTargetValue(0);
  setLinearInertionTimeCoeff(DEFAULT_INTEGRATOR_TIME);
}

void MotorPwm::setRampMode(RampMode mode)
{
  rampControl->setMode(mode);
}

void MotorPwm::setPeriodMax(int16_t periodMax)
{
  this->periodMax = periodMax;
}

void MotorPwm::updatePeripheralSetting()
{
  if (getTimerDuty() <= 0)  //INCREMENT_ANGLE_DIR_DUTY_SIGN
  {
    setPeripheralIncrementDirection();
  }
  else
  {
    setPeripheralDecrementDirection();
  }
}

void MotorPwm::setUpdateRequest()
{
  updateRequest = true;
}

void MotorPwm::calculateNewDuty()
{
  timerDuty = static_cast<int16_t>((rampControl->calculateNextSetPoint() / 100)
  * periodMax);
}

int16_t MotorPwm::getTimerDuty()
{
  return timerDuty;
}
