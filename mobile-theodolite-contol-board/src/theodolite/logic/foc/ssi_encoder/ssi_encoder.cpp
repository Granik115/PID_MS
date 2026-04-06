/*
 * ssi_encoder.cpp
 *
 *  Created on: Dec 26, 2024
 *      Author: mvm
 */

#include "ssi_encoder.h"
#include "sync/timer_channel.h"

SSIEncoder::SSIEncoder(uint32_t resolution, bool hasAlarmBit, uint32_t dataPinBBAddr, uint32_t clkDataBBAddr, bool grayCode)
{
  m_resolution = resolution;
  m_hasAlarm = hasAlarmBit;
  m_pinDataBBAddr = dataPinBBAddr;
  m_pinClkBBAddr = clkDataBBAddr;
  m_ssiBusState = SSIBusState_RESET;
  m_currentBit = 0;
  m_clkPinVal = 1;
  m_isAlarm = false;
  m_isGray = grayCode;
  m_encoderAngleRaw = 0;
  WRITE_BB(m_pinClkBBAddr, m_clkPinVal);
}

bool SSIEncoder::isrHandler()
{
  if (!m_update)
    return false;

  switch (m_ssiBusState)
  {
    case SSIBusState_RESET:
      m_clkPinVal = 0;
      m_currentBit = 0;
      m_encoderTmpData = 0;
      WRITE_BB(m_pinClkBBAddr, m_clkPinVal);
      m_ssiBusState = SSIBusState_LATCH;
      break;
    case SSIBusState_LATCH:
      m_clkPinVal = 1;
      WRITE_BB(m_pinClkBBAddr, m_clkPinVal);
      m_ssiBusState = SSIBusState_RECEIVING;
      break;
    case SSIBusState_RECEIVING:
      if (m_clkPinVal == 0)
      {
        m_encoderTmpData = (m_encoderTmpData << 1) | READ_BB(m_pinDataBBAddr);
        m_currentBit++;
      }
      m_clkPinVal = !m_clkPinVal;
      WRITE_BB(m_pinClkBBAddr, m_clkPinVal);
      if (m_currentBit > m_resolution + (uint8_t)m_hasAlarm - 1)
      {
        m_currentBit = 0;
        m_encoderData = m_encoderTmpData;
        if (m_hasAlarm)
        {
          const uint32_t alarmBitMask = 0x1 << m_resolution;
          m_isAlarm = !(m_encoderData & alarmBitMask);
          m_encoderData = m_encoderData & (~alarmBitMask);
        }
        if(m_isGray)
          m_encoderAngleRaw = grayDecode(m_encoderData);
        else
          m_encoderAngleRaw = m_encoderData;
        m_ssiBusState = SSIBusState_RELEASING;
      }
      break;
    case SSIBusState_RELEASING:
      if (m_currentBit++ >= SSI_RESET_COUNT)
      {
        m_update = false;
        m_ssiBusState = SSIBusState_RESET;
      }
      break;
    default:
      break;
  }
  return m_update;
}

void SSIEncoder::update()
{
  m_update = true;
  enableSSITimer();
  Sensor::update();
}

uint32_t SSIEncoder::grayDecode(uint32_t data)
{
  uint32_t decoded = 0;
  for (decoded = 0; data; data >>= 1)
    decoded ^= data;
  return decoded;
}

int32_t SSIEncoder::getEncoderCode()
{
  return m_encoderAngleRaw;
}

float SSIEncoder::getSensorAngle()
{
  if (m_isAlarm)
    return 0;
  return (_2PI * m_encoderAngleRaw) / (1 << m_resolution);
}

int SSIEncoder::needsSearch()
{
  return 0;
}
