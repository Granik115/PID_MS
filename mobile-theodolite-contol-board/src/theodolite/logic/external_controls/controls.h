///*(c) 2016 NIC SPbETU
// * controls.h
// *
// *      Author: mmalenko
// */

#ifndef PERIPHERAL_CONTROLS_CONTROLS_H_
#define PERIPHERAL_CONTROLS_CONTROLS_H_

#include "theodolite/logic/soft_timer/sw_timer.h"
#include "stdint.h"
#include "firmware/ring_buffer/ring_buffer.h"
#include <theodolite/logic/periph_bridge/gpio/gpio.h>
#include <firmware/queue/queue.h>

const int32_t BUTTONS_ANTIJITTER_TIME = 40;

void controlsProcessingTask(void);
void controlsScanTask(void);
void controlsCreate();
void controlsConfiguration(void);

typedef enum
{
  BUTTON_PRESSED,
  BUTTON_RELEASED
} ButtonState;

typedef enum
{
  AZIMUTH_INCREMENT_PRESSED,
  AZIMUTH_INCREMENT_RELEASED,

  AZIMUTH_DECREMENT_PRESSED,
  AZIMUTH_DECREMENT_RELEASED,

  ELEVATION_INCREMENT_PRESSED,
  ELEVATION_INCREMENT_RELEASED,

  ELEVATION_DECREMENT_PRESSED,
  ELEVATION_DECREMENT_RELEASED,

  CONTROLLER_CONNECT,
  CONTROLLER_DISCONNECT
} ControllersEvent;

#define BUTTON_PIN_PRESS_VALUE false
#define BUTTON_PIN_RELEASE_VALUE !BUTTON_PIN_PRESS_VALUE

class ButtonAbstract
{
public:
  virtual ~ButtonAbstract()
  {

  }
  virtual void update()=0;
  static ControllersEvent getEvent()
  {
    return m_controlsEventQueue.get();
  }
  static bool isEventGenerated()
  {
    return m_controlsEventQueue.getCount() != 0;
  }
protected:
  static Queue<ControllersEvent> m_controlsEventQueue;
};

class OverCurrentTriggerInterface
{
public:
  virtual void update()=0;
  virtual bool isLatch()=0;
  virtual void clear()=0;
  virtual ~OverCurrentTriggerInterface()
  {

  }
};

void overCurrentTriggersConfig();
OverCurrentTriggerInterface *getAzimuthOverCurrentTrigger();
OverCurrentTriggerInterface *getElevaiontOverCurrentTrigger();

class Button:
  public ButtonAbstract
{
public:
  Button(ControllersEvent pressEvent,
         ControllersEvent releaseEvent,
         GpioPinInterface *pin);
  virtual ~Button()
  {

  }
  void update();
  private:
  Timer m_buttonAntiJitterTimer;
  ButtonState m_oldStableState;
  ButtonState m_currentStableState;
  ButtonState m_expectedStableState;
  ControllersEvent m_pressEvent;
  ControllersEvent m_releaseEvent;
  GpioPinInterface *m_pin;
  ButtonState getButtonState(GpioPinInterface *pin);
  void antiJitter();
};

#endif /* PERIPHERAL_CONTROLS_CONTROLS_H_ */
