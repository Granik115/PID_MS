///*(c) 2016 NIC SPbETU
// * controls.c
// *
// *Author: mmalenko
// */

#include <theodolite/logic/external_controls/controls.h>
#include "stdbool.h"
#include "stdio.h"
#include <theodolite/logic/soft_timer/sw_timer.h>

Queue<ControllersEvent> ButtonAbstract::m_controlsEventQueue = Queue<ControllersEvent>(10);

void Button::antiJitter()
{
  ButtonState currentSelectButtonState = getButtonState(m_pin);
  if (currentSelectButtonState != m_expectedStableState)
  {
    m_buttonAntiJitterTimer.restart();
    m_expectedStableState = currentSelectButtonState;
  }
  else if (m_buttonAntiJitterTimer.expired())
  {
    m_currentStableState = m_expectedStableState;
  }
}

Button::Button(ControllersEvent pressEvent,
               ControllersEvent releaseEvent,
               GpioPinInterface *pin) :
  m_pressEvent(pressEvent), m_releaseEvent(releaseEvent), m_buttonAntiJitterTimer(BUTTONS_ANTIJITTER_TIME),
  m_oldStableState(BUTTON_RELEASED),
  m_currentStableState(BUTTON_RELEASED), m_expectedStableState(BUTTON_RELEASED), m_pin(pin)
{

}

ButtonState Button::getButtonState(GpioPinInterface *pin)
{
  ButtonState state = BUTTON_RELEASED;
  bool pinValue = pin->getLevel();
  if (pinValue == BUTTON_PIN_PRESS_VALUE)
  {
    state = BUTTON_PRESSED;
  }
  else if (pinValue == BUTTON_PIN_RELEASE_VALUE)
  {
    state = BUTTON_RELEASED;
  }
  return state;
}

void Button::update()
{
  antiJitter();
  if ((m_currentStableState == BUTTON_PRESSED) && (m_oldStableState == BUTTON_RELEASED))
  {
    m_controlsEventQueue.put(m_pressEvent);
  }
  else if ((m_currentStableState == BUTTON_RELEASED) && (m_oldStableState == BUTTON_PRESSED))
  {
    m_controlsEventQueue.put(m_releaseEvent);
  }
  m_oldStableState = m_currentStableState;
}

