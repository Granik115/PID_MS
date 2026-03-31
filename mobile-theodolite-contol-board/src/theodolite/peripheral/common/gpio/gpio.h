///*
// * gpio.h
// *
// *      Author: mmalenko
// *  (c) 2016 NIC SPbETU
//*/

#ifndef PERIPHERAL_GPIO_GPIO_H_
#define PERIPHERAL_GPIO_GPIO_H_

#include <theodolite/logic/periph_bridge/gpio/gpio.h>
#include <theodolite/logic/external_controls/controls.h>
#include <theodolite/peripheral/hardware.h>

const uint16_t PIN_HIGH_VALUE = 1;
const uint16_t PIN_LOW_VALUE = 0;

GpioPinInterface* getPin(GpioPins pinName);

class GpioPin:
  public GpioPinInterface
{
public:
  GpioPin(volatile uint16_t * const gpio, uint16_t gpioPin);
  void setHigh();
  void setLow();
  bool getLevel();
  void setLevel(bool);
  private:
  volatile uint16_t *m_gpio;
  uint16_t m_gpioPin;
};

#endif /* PERIPHERAL_GPIO_GPIO_H_ */
