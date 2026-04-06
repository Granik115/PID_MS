/*
 * encoder_protocol.cpp
 *
 *  Created on: 19 мар. 2019 г.
 *      Author: mmalenko
 */

#include <theodolite/logic/encoder_board_main/encoder_board_protocol/encoder_protocol.h>
#include <theodolite/logic/encoder_board_main/encoder/encoder.h>
#include "theodolite/logic/periph_bridge/uart.h"

const uint_least8_t INTERFACE_MARKER = 0xC0;

EncoderBoardProtocol::EncoderBoardProtocol(InclinometerInterface &inclinometer, EncoderInterface &azimuthEncoder,
                                           EncoderInterface &elevationEncoder,
                                           ConnectionInterface &interface) :
  m_azimuthEncoder(azimuthEncoder), m_elevationEncoder(elevationEncoder), m_interface(interface),
  m_inclinometer(inclinometer), m_sendRequest(false), m_bufferPointer(0)
{

}

void EncoderBoardProtocol::setSendRequest()
{
  m_sendRequest = true;
}

bool EncoderBoardProtocol::sendStateOnRequest()
{
  if (m_sendRequest)
  {
    m_sendRequest = false;
    return sendStateWithoutRequest();
  }
  return false;
}

void EncoderBoardProtocol::putValueToBuffer(uint32_t value, uint_least8_t *buffer, uint32_t valueSize)
{
  for (uint32_t i = 0; i < valueSize; i++)
  {
    buffer[m_bufferPointer++] = (value >> (i * 8)) & 0xFF;
  }
}

uint32_t EncoderBoardProtocol::getCode(int32_t mills)
{
  static const int32_t ENCODER_MILL_SEC_MIN_VALUE = 4096000;
  static const int32_t ENCODER_MIIL_SEC_PER_CODE = 125;
  return (mills + ENCODER_MILL_SEC_MIN_VALUE) / ENCODER_MIIL_SEC_PER_CODE;
}

bool EncoderBoardProtocol::sendStateWithoutRequest()
{
  const uint_least8_t MAX_ENCODER_MESSAGE_SIZE = 12;
  uint_least8_t buffer[MAX_ENCODER_MESSAGE_SIZE];
  m_bufferPointer = 0;
  const uint32_t azimuth = m_azimuthEncoder.getCount(); // * (720000 - 1)) / 2047;
  uint32_t size = 3;
  putValueToBuffer(azimuth, buffer, size);
  const uint32_t elevation = m_elevationEncoder.getCount(); // * (720000 - 1)) / 2047;
  putValueToBuffer(elevation, buffer, size);
  size = 2;
  putValueToBuffer(getCode(m_inclinometer.getXMills()), buffer, size);
  putValueToBuffer(getCode(m_inclinometer.getYMills()), buffer, size);
  size = 1;
  putValueToBuffer(static_cast<uint_least8_t>(m_azimuthEncoder.getAxisPosition()), buffer, size);
  putValueToBuffer(static_cast<uint_least8_t>(m_elevationEncoder.getAxisPosition()), buffer, size);
  Data<uint_least8_t> data = Data<uint_least8_t>(buffer, MAX_ENCODER_MESSAGE_SIZE);
  uint_least8_t linkBuffer[MAX_FRAME_SIZE];
  uint16_t writtenSize = uartLinkEncode(&data, linkBuffer, false, 0xC0);
  data.m_data = linkBuffer;
  data.m_size = writtenSize;
  return m_interface.send(&data);
}
