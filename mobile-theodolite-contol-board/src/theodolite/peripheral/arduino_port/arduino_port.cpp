/*
 * arduino_port.cpp
 *
 *  Created on: Aug 30, 2024
 *      Author: mvm
 */
#include "Arduino.h"

/*
 * Parameters
 pin
 - the pin we want to use for an interrupt.
 Returns
 The pin to interrupt (e.g.2)+
 If pin is not available for interrupt, returns-1
 * */
int digitalPinToInterrupt(int pin)
{
  return -1;
}

int analogRead(int pin)
{
  return 0;
}

void digitalWrite(uint32_t ulPin, uint32_t ulVal)
{

}

void analogWrite(uint32_t ulPin, uint32_t ulValue)
{

}

bool digitalRead(int pin)
{
  return false;
}
