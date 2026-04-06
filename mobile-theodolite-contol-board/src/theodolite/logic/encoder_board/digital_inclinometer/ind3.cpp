/*
 * ind3.cpp
 *
 *  Created on: Jan 15, 2025
 *      Author: mvm
 */

#include "firmware/data/data.h"
#include "encoder_board/digital_inclinometer/ind3.h"
#include "math.h"
#include "assert/assert_parameter.h"

const uint8_t testMeasureSeq[][14] =
{
  {0x7E, 0x9B, 0x01, 0x01, 0x6A, 0x77, 0x80, 0x38, 0xC2, 0x00, 0xFC, 0x7E},
  {0x7E, 0x9B, 0x01, 0x01, 0x07, 0x09, 0x01, 0x00, 0x68, 0x81, 0x7D, 0x5D, 0x7E}, //-360,000 сек. 265,027 сек.
  {0x7E, 0x9B, 0x01, 0x01, 0x07, 0x09, 0x01, 0x00, 0x68, 0x81, 0x7D, 0x5D, 0x7D, 0x7E} //wrong escape
};

InD3Inclinometer::InD3Inclinometer(ConnectionInterface &interface, uint8_t address) :
  m_interface(interface),
  m_dataReliableTimer(DATA_RELIABLE_TIMEOUT)
{
  m_address = address;
  m_xmills = 0;
  m_ymills = 0;
  m_isDataReliable = false;
  m_packetBufferPointer = 0;
  isStartDetect = false;
}

bool InD3Inclinometer::decodeMessage(uint8_t *packetBuffer, size_t len)
{
  uint8_t out[len];
  size_t outLen = decodeEscaping(packetBuffer, len, out);
  if (outLen == 0 || (out[outLen - 1] != crc(out, outLen - 1)))
    return false;
  InD3MessageHeader *header = (InD3MessageHeader*)out;
  switch (header->requiestType)
  {
    case GET_MEASURE_ID:
      updateMeasures(decodeMeasureMessage(out, outLen));
      return true;
    default:
      break;
  }
  return false;
}

size_t InD3Inclinometer::receiveFrame(uint8_t byte)
{
  if (byte == FRAME_DELIMITER)
  {
    if (isStartDetect)
    {
      const size_t dataLen = m_packetBufferPointer;
      m_packetBufferPointer = 0;
      isStartDetect = false;
      return dataLen;
    }
    else
      isStartDetect = true;
  }
  else if (isStartDetect)
  {
    m_packetBuffer[m_packetBufferPointer++] = byte;
    m_packetBufferPointer &= PACKET_BUFFER_MASK;
  }
  return 0;
}

void InD3Inclinometer::update()
{
  while (m_interface.getRxQueue()->getCount())
  {
    const size_t payloadLen = receiveFrame(m_interface.getRxQueue()->get());
    if (!payloadLen)
      continue;

    if (decodeMessage(m_packetBuffer, payloadLen))
    {
      m_isDataReliable = true;
      m_dataReliableTimer.stop();
    }
    else
    {
      if (m_dataReliableTimer.isRunning() && m_dataReliableTimer.expired())
      {
        m_isDataReliable = false;
        theodolite_messages_Error error =
        {theodolite_messages_Device_INCLINOMETER, 1,
          {theodolite_messages_ErrorType_ERROR_INCLINOMETER_OFFLINE}};
        getErrorStorage().sendError(error);
        m_dataReliableTimer.restart();
      }
      else
        m_dataReliableTimer.start();
    }
  }
  sendMeasureDataRequest();
}

uint8_t InD3Inclinometer::crc(uint8_t *dataIn, size_t dataInLen)
{
  uint8_t crc = 0;
  for (size_t i = 0; i < dataInLen; i++)
    crc ^= dataIn[i];
  return crc;
}

int32_t InD3Inclinometer::getMills(AxisMeasure measure)
{
  int32_t sign = measure.integerData.measure.sign == INC_SIGN_POSITIVE ? 1 : -1;
  uint32_t koeff = measure.integerData.measure.dimension == INC_DIMENSION_SEC ? 1000 : 60000;
  uint32_t fractionMills = (uint32_t)round((double)measure.fractionPart * koeff / 256);
  uint32_t integerMills = measure.integerData.measure.integer * koeff;
  return sign * (integerMills + fractionMills);
}

void InD3Inclinometer::updateMeasures(InD3MeasureMessage *measures)
{
  int32_t x = getMills(measures->x_measure);
  if (x > 360000 || x < -360000)
  {
    volatile uint32_t i = 0;
    i++;
  }
  m_xmills = x;
  int32_t y = getMills(measures->y_measure);
  if (y > 360000 || y < -360000)
  {
    volatile uint32_t i = 0;
    i++;
  }
  m_ymills = y;
}

int32_t InD3Inclinometer::getXMills()
{
  return m_xmills;
}

int32_t InD3Inclinometer::getYMills()
{
  return m_ymills;
}

bool InD3Inclinometer::isDataReliable()
{
  return m_isDataReliable;
}

uint32_t InD3Inclinometer::getEscapeCount(uint8_t *dataIn, size_t dataInLen)
{
  uint32_t count = 0;
  for (size_t i = 0; i < dataInLen; i++)
    if (dataIn[i] == ESCAPE_BYTE)
      count++;

  return count;
}

InD3MeasureMessage* InD3Inclinometer::decodeMeasureMessage(uint8_t *data, size_t size)
{
  return (InD3MeasureMessage*)data;
}

int32_t InD3Inclinometer::getFrameDelimeterIndex(uint8_t *dataIn, size_t dataInLen)
{
  for (int32_t inIndex = 0; inIndex < (int32_t)dataInLen; inIndex++)
    if (dataIn[inIndex] == FRAME_DELIMITER)
      return inIndex;
  return -1;
}

size_t InD3Inclinometer::decodeEscaping(uint8_t *dataIn, size_t dataInLen, uint8_t *dataOut)
{
  size_t outIndex = 0;
  for (size_t inIndex = 0; inIndex < dataInLen; inIndex++)
  {
    if (dataIn[inIndex] == ESCAPE_BYTE)
    {
      if (inIndex + 1 < dataInLen)
        dataOut[outIndex++] = dataIn[++inIndex] ^ ESCAPE_MASK;
      else
        //wrong escape in buffer
        return 0;
    }
    else
      dataOut[outIndex++] = dataIn[inIndex];
  }
  return outIndex;
}

size_t InD3Inclinometer::encodeEscaping(uint8_t *dataIn, size_t dataInLen, uint8_t *dataOut)
{
  size_t outIndex = 0;
  for (size_t inIndex = 0; inIndex < dataInLen; inIndex++)
  {
    if (dataIn[inIndex] == FRAME_DELIMITER || dataIn[inIndex] == ESCAPE_BYTE)
    {
      dataOut[outIndex++] = ESCAPE_BYTE;
      dataOut[outIndex++] = dataIn[inIndex] ^ ESCAPE_MASK;
    }
    else
      dataOut[outIndex++] = dataIn[inIndex];
  }
  return outIndex;
}

size_t InD3Inclinometer::encodeFrame(uint8_t *dataIn, size_t dataInLen, uint8_t *dataOut)
{
  const uint32_t payloadLen = encodeEscaping(dataIn, dataInLen, dataOut + 1);
  dataOut[0] = FRAME_DELIMITER;
  dataOut[payloadLen + 1] = FRAME_DELIMITER;
  return payloadLen + 2;
}

bool InD3Inclinometer::sendMeasureDataRequest()
{
  uint8_t packet[REQUEST_MEASURE_DATA_LEN] =
  {PROTOCOL_ID_MAIN, GET_MEASURE_ID, m_address, crc(packet, REQUEST_MEASURE_DATA_LEN - 1)};
  uint8_t packetOut[REQUEST_MEASURE_DATA_LEN * 2 + 2] = {};
  size_t packetLen = encodeFrame(packet, REQUEST_MEASURE_DATA_LEN, packetOut);
  Data<uint8_t> data =
  {packetOut, packetLen};
  return m_interface.send(&data);
}

