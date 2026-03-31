/*
 * internal_eeprom.h
 *
 *  Created on: 12 июл. 2019 г.
 *      Author: mmalenko
 *  (c) 2019 NIC SPbETU
 */

#ifndef SRC_THEODOLITE_PERIPHERAL_MEMORY_INTERNAL_EEPROM_H_
#define SRC_THEODOLITE_PERIPHERAL_MEMORY_INTERNAL_EEPROM_H_

#include "theodolite/logic/memory/memory_interface.h"

class InternalEeprom:
  public MemoryInterface
{
public:
  InternalEeprom()
  {

  }
  bool read(Data<uint_least8_t>* data, uint32_t offset)
  {
    return false;
  }
  bool write(Data<uint_least8_t> data, uint32_t offset)
  {
    return false;
  }
  //Full erase wo save
  bool erase(uint32_t size, uint32_t offset)
  {
    return false;
  }
  bool testing()
  {
    return true;
  }
};

#endif /* SRC_THEODOLITE_PERIPHERAL_MEMORY_INTERNAL_EEPROM_H_ */
