///*
// * BootloaderCommandsInterface.h
// *
// *  Created on: 25 дек. 2018 г.
// *      Author: mmalenko
// // * (c) 2018 NIC SPbETU
// */

#ifndef SRC_THEODOLITE_LOGIC_BOOTLOADER_BOOTLOADER_COMMANDS_INTERFACE_H_
#define SRC_THEODOLITE_LOGIC_BOOTLOADER_BOOTLOADER_COMMANDS_INTERFACE_H_

#include <theodolite/logic/bootloader/third-party/boot.pb.h>
#include <firmware/data/data.h>

class BootloaderCommandsInterface
{
public:
  virtual void setInIdle()=0;
  virtual State getState(SectorType)=0;
  virtual bool processFirmwareSegment(SectorType, FirmwareSegment &segment)=0;
  virtual bool verifyCrc(SectorType sector, uint32_t crc)=0;
  virtual void startApplication()=0;
  virtual bool erase(SectorType sector)=0;
  virtual void update()=0;
  virtual const char* getBootloaderVersionString()=0;
  virtual const char* getApplicationVersionString()=0;
  virtual void getApplicationVersionString(Data<char>&)=0;
  virtual PlatformType getBootloaderPlatformType()=0;
};

#endif /* SRC_THEODOLITE_LOGIC_BOOTLOADER_BOOTLOADER_COMMANDS_INTERFACE_H_ */
