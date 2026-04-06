///*
// * brakes.h
// *
// *  Created on: 24 янв. 2017 г.
// *      Author: mmalenko
// * (c) 2017 NIC SPbETU
// */

#ifndef LOGIC_BRAKES_BRAKES_H_
#define LOGIC_BRAKES_BRAKES_H_

#include <theodolite/logic/periph_bridge/gpio/gpio.h>
#include <theodolite/logic/soft_timer/sw_timer.h>

typedef enum
{
  ACTIVE,
  INACTIVE,
  UNSTABLE
} BrakeState;

class BrakesInterface
{
public:
  virtual void activate()=0;
  virtual void deactivate()=0;
  virtual BrakeState getBrakeState()=0;
  virtual ~BrakesInterface()
  {

  }
};

class Brake:
  public BrakesInterface
{
  bool m_expectedState;
  bool m_state;
  Timer m_brakeHoldTimer;
  GpioPinInterface *m_pin;
  void setState(bool state);
  public:
  void activate();
  void deactivate();
  BrakeState getBrakeState();
  explicit Brake(GpioPinInterface *pin);
  ~Brake();
};

#endif /* LOGIC_BRAKES_BRAKES_H_ */
