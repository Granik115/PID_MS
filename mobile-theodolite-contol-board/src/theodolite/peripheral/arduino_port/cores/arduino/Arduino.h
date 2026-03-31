/*
 * Arduino.h
 *
 *  Created on: Aug 26, 2024
 *      Author: mvm
 */

#ifndef THEODOLITE_PERIPHERAL_ARDUINO_H_
#define THEODOLITE_PERIPHERAL_ARDUINO_H_

#include "stdint.h"
#include "math.h"
#include "main.h"
#include "wiring.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
// Weak empty variant initialization function.
// May be redefined by variant files.
extern void initVariant() __attribute__((weak));
extern void setup(void) ;
extern void loop(void) ;
void yield(void);
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

int analogRead(int pin);
void analogWrite(uint32_t ulPin, uint32_t ulValue);
int digitalPinToInterrupt(int pin);
bool digitalRead(int pin);
void digitalWrite(uint32_t ulPin, uint32_t ulVal);

#endif /* THEODOLITE_PERIPHERAL_ARDUINO_H_ */
