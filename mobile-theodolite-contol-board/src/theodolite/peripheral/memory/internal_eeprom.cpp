/*
 * internal_eeprom.cpp
 *
 *  Created on: 12 июл. 2019 г.
 *      Author: mmalenko
 * (c) 2019 NIC SPbETU
 */

#include "theodolite/peripheral/memory/internal_eeprom.h"

InternalEeprom internalEeprom;

MemoryInterface& getInternalEeprom()
{
  return internalEeprom;
}

