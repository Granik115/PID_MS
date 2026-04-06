/*
 * bootloader_commands.cpp
 *
 *  Created on: 3 июн. 2019 г.
 *      Author: mmalenko
 */

#include <stdint.h>
#include <theodolite/peripheral/bootloader_common/version.h>
#include "theodolite/logic/bootloader/bootloader_commands.h"

const int32_t APPLICATION_START_TIME = 1000;
const uint32_t APP_INDEX = 0;
const uint32_t BOOTLOADER_INDEX = 1;

void BootloaderCommands::setInIdle()
{
  m_idleFlag = true;
}

BootloaderCommands::BootloaderCommands(PlatformInterface &platform) :
  m_platform(platform), m_stateTimer(APPLICATION_START_TIME), m_idleFlag(false)
{
  State tmpState = State_init_default;
  m_state[0] = tmpState;
  m_state[1] = tmpState;
  m_stateTimer.restart();
}

PlatformType BootloaderCommands::getBootloaderPlatformType()
{
  return m_platform.getPlatformType();
}

int16_t BootloaderCommands::getSectorIndex(SectorType sector)
{
  int16_t sectorNumber = -1;
  if (sector == SectorType_APPLICATION_SECTOR)
  {
    sectorNumber = APP_INDEX;
  }
  else if (sector == SectorType_BOOTLOADER_SECTOR)
  {
    sectorNumber = BOOTLOADER_INDEX;
  }
  return sectorNumber;
}

const char* BootloaderCommands::getBootloaderVersionString()
{
  return BOOTLOADER_VERSION;
}

void BootloaderCommands::getApplicationVersionString(Data<char>&data)
{
  m_platform.getApplicationVersion(data);
}

const char* BootloaderCommands::getApplicationVersionString()
{
  return m_platform.getApplicationVersion();
}

State BootloaderCommands::getState(SectorType sector)
{
  const int16_t index = getSectorIndex(sector);
  if (m_state[index].state == StateType_WAIT_COMMAND)
  {
    setInIdle();
    m_state[index].state = StateType_IDLE;
  }
  return m_state[index];
}

bool BootloaderCommands::processFirmwareSegment(SectorType sector, FirmwareSegment &segment)
{
  const uint16_t sectorIndex = getSectorIndex(sector);
  m_state[sectorIndex].last_segment_head.operation = segment.firmware_segment_head.operation;

  bool isSequenceCorrect = segment.firmware_segment_head.segment_end_number
  == m_state[sectorIndex].last_segment_head.segment_end_number + segment.firmware_segment_data.size;

  if (!isSequenceCorrect)
  {
    m_state[sectorIndex].has_error = true;
    m_state[sectorIndex].error.error = ErrorType_SEGMENT_SEQUENCE_ERROR;
    return false;
  }
  if (m_platform.isSectorOverflow(sector, segment.firmware_segment_head.segment_end_number))
  {
    m_state[sectorIndex].has_error = true;
    m_state[sectorIndex].error.error = ErrorType_SECTOR_OVERFLOW_ERROR;
    return false;
  }
  if (segment.has_last_segment)
  {
    m_state[sectorIndex].last_segment_head.segment_end_number = 0;
  }
  else
  {
    m_state[sectorIndex].last_segment_head.segment_end_number = segment.firmware_segment_head.segment_end_number;
  }
  m_state[sectorIndex].has_error = !m_platform.processFirmwareSegment(segment.firmware_segment_head.operation, sector,
                                                                      m_state[sectorIndex].error,
                                                                      segment.firmware_segment_head.segment_end_number,
                                                                      segment.firmware_segment_data.bytes,
                                                                      segment.firmware_segment_data.size);
  if (m_state[sectorIndex].has_error && m_state[sectorIndex].error.has_status)
  {
    m_state[sectorIndex].state = StateType_ERROR;
  }
  return !m_state[sectorIndex].has_error;
}

void BootloaderCommands::startApplication()
{
  m_platform.startApplication();
}

bool BootloaderCommands::erase(SectorType sector)
{
  return m_platform.erase(sector);
}

bool BootloaderCommands::verifyCrc(SectorType sector, uint32_t crc)
{
  return m_platform.verifyCrc(sector, crc);
}

void BootloaderCommands::update()
{
  if (!m_idleFlag)
  {
    if (m_stateTimer.expired())
    {
      startApplication();
    }
  }
}

