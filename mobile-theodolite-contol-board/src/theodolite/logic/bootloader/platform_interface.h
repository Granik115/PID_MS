/*
 * platform_interface.h
 *
 *  Created on: 3 июн. 2019 г.
 *      Author: mmalenko
 */

#ifndef SRC_THEODOLITE_LOGIC_BOOTLOADER_PLATFORM_INTERFACE_H_
#define SRC_THEODOLITE_LOGIC_BOOTLOADER_PLATFORM_INTERFACE_H_

#include "theodolite/logic/bootloader/third-party/boot.pb.h"
#include "firmware/data/data.h"

class PlatformInterface
{
public:
  virtual bool isSectorOverflow(SectorType sector, uint32_t segment_end_number)=0;
  virtual PlatformType getPlatformType()=0;
  virtual bool verifyCrc(SectorType sector, uint32_t crc)=0;
  virtual bool processFirmwareSegment(OperationType, SectorType, Error &err,
                                      uint32_t segment_end_number,
                                      pb_byte_t *data, pb_size_t size)=0;
  virtual void startApplication()=0;
  virtual bool erase(SectorType sector)=0;
  virtual char *getApplicationVersion()=0;
  virtual void getApplicationVersion(Data<char>&)=0;
};

PlatformInterface &getPlatform();

#endif /* SRC_THEODOLITE_LOGIC_BOOTLOADER_PLATFORM_INTERFACE_H_ */
