///*
// * gpio.cpp
// *
// *  Created on: 12 мар. 2019 г.
// *      Author: mmalenko
//  (c) 2019 NIC SPbETU
// */

#include <theodolite/peripheral/common/gpio/gpio.h>

void GpioPin::setLow()
{
  uint16_t gpioData = *m_gpio;
  gpioData &= ~m_gpioPin;
  *m_gpio = gpioData;
}

void GpioPin::setHigh()
{
  uint16_t gpioData = *m_gpio;
  gpioData |= m_gpioPin;
  *m_gpio = gpioData;
}

GpioPin::GpioPin(volatile uint16_t * const gpio, uint16_t gpioPin)
{
  this->m_gpio = gpio;
  this->m_gpioPin = gpioPin;
}

void GpioPin::setLevel(bool level)
{
  if (level)
  {
    setHigh();
  }
  else
  {
    setLow();
  }
}

bool GpioPin::getLevel()
{
  uint16_t gpioData = *m_gpio;

  return gpioData & m_gpioPin;
}
