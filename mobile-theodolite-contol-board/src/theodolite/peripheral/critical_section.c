/*
 * critical_section.cpp
 *
 *  Created on: 14 мая 2020 г.
 *      Author: maxim
 *  (c) 2020 NIC SPbETU
 */
#include "critical_section.h"
#include "cmsis_gcc.h"

void criticalSectionBegin()
{
  __disable_irq();
}

void criticalSectionEnd()
{
  __enable_irq();
}

#define  NUMBER_OF_PROCESS  2
uint8_t turn;
bool interested[NUMBER_OF_PROCESS];

bool enterRegion(uint8_t process)
{
  uint8_t other;
  other = 1 - process;
  interested[process] = true;
  turn = process;
  if (turn == process && interested[other] == true)
  {
    interested[process] = false;
    return false;
  }
  return true;
}

void leaveRegion(uint8_t process)
{
  interested[process] = false;
}
