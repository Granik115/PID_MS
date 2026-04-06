/*
 * bootloader_commands.h
 *
 *  Created on: 3 июн. 2019 г.
 *      Author: mmalenko
 */

#ifndef SRC_THEODOLITE_LOGIC_BOOTLOADER_BOOTLOADER_COMMANDS_H_
#define SRC_THEODOLITE_LOGIC_BOOTLOADER_BOOTLOADER_COMMANDS_H_

#include "theodolite/logic/bootloader/bootloader_commands_interface.h"
#include "theodolite/logic/soft_timer/sw_timer.h"
#include "theodolite/logic/bootloader/platform_interface.h"

class BootloaderCommands:
  public BootloaderCommandsInterface
{
  Timer m_stateTimer;
  State m_state[2];
  bool m_idleFlag;
  PlatformInterface &m_platform;
  int16_t getSectorIndex(SectorType sector);
  public:
  BootloaderCommands(PlatformInterface &platform);
  void setInIdle();
  State getState(SectorType sector);
  bool erase(SectorType sector);
  bool processFirmwareSegment(SectorType sector, FirmwareSegment &segment);
  bool verifyCrc(SectorType sector, uint32_t crc);
  void startApplication();
  void update();
  const char* getBootloaderVersionString();
  const char* getApplicationVersionString();
  void getApplicationVersionString(Data<char>&data);
  PlatformType getBootloaderPlatformType();
};

#endif /* SRC_THEODOLITE_LOGIC_BOOTLOADER_BOOTLOADER_COMMANDS_H_ */
