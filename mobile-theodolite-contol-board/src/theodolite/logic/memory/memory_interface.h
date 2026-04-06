///*(c) 2018 NIC SPbETU
// * memory_interface.h
// *
// *  Created on: 21 июня 2018 г.
// *      Author: mmalenko
// */

#ifndef SRC_THEODOLITE_PERIPHERAL_MEMORY_MEMORY_INTERFACE_H_
#define SRC_THEODOLITE_PERIPHERAL_MEMORY_MEMORY_INTERFACE_H_

#include <firmware/link_level/uart_link.h>

class MemoryInterface
{
public:
  virtual ~MemoryInterface()
  {

  }
  virtual bool erase(uint32_t size, uint32_t startAddress)=0;
  virtual bool read(Data<uint_least8_t>*, uint32_t startAddress)=0;
  virtual bool write(Data<uint_least8_t>, uint32_t startAddress)=0;
  virtual bool testing()=0;
};

MemoryInterface * getFlash();
MemoryInterface *getEeprom();
MemoryInterface &getInternalEeprom();

#endif /* SRC_THEODOLITE_PERIPHERAL_MEMORY_MEMORY_INTERFACE_H_ */
