///*
// * gpio.c
// *
// *
// *      Author: mmalenko
// *  (c) 2016 NIC SPbETU
// */

#include "stddef.h"
#include "theodolite/peripheral/hal.h"
#include "theodolite/logic/periph_bridge/gpio/gpio.h"
#include "theodolite/peripheral/common/gpio/gpio.h"

bool isTestMode()
{
  return false;
}

OverCurrentTriggerInterface* getAzimuthOverCurrentTrigger()
{
  return NULL;
}
OverCurrentTriggerInterface* getElevaiontOverCurrentTrigger()
{
  return NULL;
}

void gpioConfiguration()
{

}

static const uint32_t PIN_ARRAY_SIZE = CAMERA_TX_SOFT_PIN + 1;

static GpioPinInterface *pinArray[PIN_ARRAY_SIZE] =
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

GpioPinInterfaceDummy dummyGpio;

GpioPinInterface* getPin(GpioPins pinName)
{
  return &dummyGpio;
}

