///*
// * brakes.cpp
// *
// *  Created on: 24 янв. 2017 г.
// *      Author: mmalenko
// *   (c) 2017 NIC SPbETU
// */

#include "theodolite/logic/axis_mover/brakes.h"
#include "theodolite/peripheral/hardware.h"

const uint32_t BRAKE_HOLD_TIME_MS = 100;

void Brake::setState(bool state)
{
  if (m_expectedState != state)
  {
    m_brakeHoldTimer.start();
    m_brakeHoldTimer.restart();
    m_expectedState = state;
    m_pin->setLevel(state);
  }
}

void Brake::activate()
{
  setState(BRAKE_ACTIVE_STATE);
}
void Brake::deactivate()
{
  setState(BRAKE_INACTIVE_STATE);
}

BrakeState Brake::getBrakeState()
{
  BrakeState state = UNSTABLE;
  if (m_brakeHoldTimer.isRunning() && m_brakeHoldTimer.expired())
  {
    m_brakeHoldTimer.stop();
    m_state = m_expectedState;
  }
  if (m_state == m_expectedState)
  {
    if (m_state == BRAKE_ACTIVE_STATE)
    {
      state = ACTIVE;
    }
    else
    {
      state = INACTIVE;
    }
  }
  else
  {
    state = UNSTABLE;
  }
  return state;
}


Brake::Brake(GpioPinInterface *pin) :
  m_pin(pin), m_brakeHoldTimer(BRAKE_HOLD_TIME_MS), m_state(BRAKE_INACTIVE_STATE), m_expectedState(BRAKE_INACTIVE_STATE)
{
  m_pin->setLevel(BRAKE_INACTIVE_STATE);
}

Brake::~Brake()
{

}
