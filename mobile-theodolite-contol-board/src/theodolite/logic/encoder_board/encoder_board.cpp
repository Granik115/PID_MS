///*
// * encoder_board_cpp.cpp
// *
// *      Author: mmalenko
// * (c) 2016 NIC SPbETU
// */

#include <theodolite/logic/assert/assert_parameter.h>
#include <theodolite/logic/encoder_board/encoder_board.h>
#include <stdlib.h>

//INTERVAL = SPEED_WINDOW*INTERVAL_SPEED_CALC_DIVIDER ms
static const uint32_t INTERVAL_SPEED_QUEUE_SIZE = 50;
static const int32_t AXIS_OVERLOAD_DETECTION_THRESHOLD = 20000; //порог разницы кода при обнулении датчика

void EncoderBoard::sendEncoderBoardCodeRequest(void)
{
  m_interface.send(ENCODER_BOARD_CODE_REQUEST_MARKER);
}

theodolite_messages_AxisPosition EncoderBoard::getAzimuthPosition()
{
  return m_azimuthAxisPosition;
}

theodolite_messages_AxisPosition EncoderBoard::getElevationPosition()
{
  return m_elevationAxisPosition;
}

/*calculate movement per one update (1ms)*/
void EncoderBoard::calculateImmediateSpeed()
{
  m_azimuthImmediateSpeed = m_azimuth - m_previousAzimuth;
  m_previousAzimuth = m_azimuth;
  m_elevationImmediateSpeed = m_elevation - m_previousElevation;
  m_previousElevation = m_elevation;
}

uint32_t EncoderBoard::calculateMaximumIntervalSpeed(QueueInterface<uint32_t> &speedQueue, int32_t immediateSpeed)
{
  if (speedQueue.getCount() == (INTERVAL_SPEED_QUEUE_SIZE - 1))
  {
    speedQueue.remove(1);
  }
  speedQueue.put(labs(immediateSpeed));
  uint32_t maximumSpeed = 0;
  const uint32_t queueCount = speedQueue.getCount() - 1;
  for (uint32_t i = 0; i < queueCount; ++i)
  {
    const uint32_t speed = speedQueue.show(i);
    if (speed > maximumSpeed)
    {
      maximumSpeed = speed;
    }
  }
  return maximumSpeed;
}

bool EncoderBoard::isAngleOverloaded(int32_t immediateSpeed)
{
  bool angleOverloadPredicate = false;
  if (labs(immediateSpeed) > AXIS_OVERLOAD_DETECTION_THRESHOLD)
  {
    angleOverloadPredicate = true;
  }
  return angleOverloadPredicate;
}

void EncoderBoard::calculateAzimuthAxisPosition()
{
  if (m_azimuthImmediateSpeed > AXIS_OVERLOAD_DETECTION_THRESHOLD)
  {
    m_azimuthAxisPosition =
    theodolite_messages_AxisPosition_OVER_ZERO_POSITION;
  }
  else if (m_azimuthImmediateSpeed
  < AXIS_OVERLOAD_DETECTION_THRESHOLD * (-1))
  {
    m_azimuthAxisPosition =
    theodolite_messages_AxisPosition_NORMAL_POSITION;
  }
}

void EncoderBoard::calculateElevationAxisPosition()
{
  if (m_elevationImmediateSpeed > AXIS_OVERLOAD_DETECTION_THRESHOLD)
  {
    m_elevationAxisPosition =
    theodolite_messages_AxisPosition_OVER_ZERO_POSITION;
  }
  else if (m_elevationImmediateSpeed
  < AXIS_OVERLOAD_DETECTION_THRESHOLD * (-1))
  {
    m_elevationAxisPosition =
    theodolite_messages_AxisPosition_NORMAL_POSITION;
  }
}

void EncoderBoard::updateWithoutRequest()
{
  decodeBinary();
  calculateImmediateSpeed();
  if (m_divider == 0)
  {
    m_azimuthMaximumIntervalSpeed = calculateMaximumIntervalSpeed(
                                                                  m_azimuthSpeedQueue,
                                                                  m_azimuthImmediateSpeed);
    m_elevationMaximumIntervalSpeed = calculateMaximumIntervalSpeed(
                                                                    m_elevationSpeedQueue,
                                                                    m_elevationImmediateSpeed);
    m_divider = INTERVAL_SPEED_CALC_DIVIDER;
  }
  --m_divider;
#ifndef NEW_ENCODER_BOARD
  calculateAzimuthAxisPosition();
  calculateElevationAxisPosition();
#endif
}

void EncoderBoard::updateOnRequest()
{
  if (m_updateRequest)
  {
    m_updateRequest = false;
    updateWithoutRequest();
  }
}

#ifndef NEW_ENCODER_BOARD
bool EncoderBoard::decodeBinary()
{
  sendEncoderBoardCodeRequest();
  QueueInterface<uint_least8_t> *rxQueue = m_interface.getRxQueue();
  if (!rxQueue)
    return false;
  if (rxQueue->getCount()
  >= ENCODER_BOARD_CODE_LENGTH)
  {
    uint_least8_t byte = rxQueue->show(0);
    if (byte == ENCODER_BOARD_CODE_REPLY_MARKER)
    {
      uint_least8_t crc = rxQueue->show(ENCODER_BOARD_CODE_CRC_SHIFT);
      if (crc
      == calculateEncoderCodeCrc(rxQueue, ENCODER_BOARD_CODE_LENGTH - 1))
      {
        m_parseTimeout.restart();
        m_dataReliable = true;
        rxQueue->remove(1);
        if (!parseEncoderCode(rxQueue))
        {
          theodolite_messages_Error error =
          {theodolite_messages_Device_INCLINOMETER, 1,
            {theodolite_messages_ErrorType_ERROR_INCLINOMETER_PACKET_ERROR_DECODE}};
          getErrorStorage().sendError(error);
        }
        return true;
      }
      else
      {
        rxQueue->remove(1);
      }
    }
    else
    {
      rxQueue->remove(1);
    }
  }
  if (m_parseTimeout.expired())
  {
    m_parseTimeout.restart();
    m_dataReliable = false;
    theodolite_messages_Error error =
    {theodolite_messages_Device_INCLINOMETER, 1,
      {theodolite_messages_ErrorType_ERROR_INCLINOMETER_OFFLINE}};
    getErrorStorage().sendError(error);
  }
  return false;
}
#else
bool EncoderBoard::decodeBinary()
{
  const uint16_t MAX_ENCODER_MESSAGE_SIZE = 16;
  uint_least8_t dataBuffer[MAX_ENCODER_MESSAGE_SIZE];
  Data<uint_least8_t> data = Data<uint_least8_t>(dataBuffer,
  sizeof(dataBuffer));
  ParserStatus status;
  do
  {
    status = getUartLinkData(m_interface.getRxQueue(), &data, false, 0xC0);
  }while (status != PARSER_STATUS_SUCCESS
  && status != PARSER_STATUS_NO_ENOUGH_DATA);
  if (status == PARSER_STATUS_SUCCESS)
  {
    m_parseTimeout.restart();
    m_dataReliable = true;
    parseEncoderCode(data);
    return true;
  }
  if (m_parseTimeout.expired())
  {
    m_parseTimeout.restart();
    m_dataReliable = false;
    theodolite_messages_Error error =
    {
      theodolite_messages_Device_INCLINOMETER, 1,
      { theodolite_messages_ErrorType_ERROR_INCLINOMETER_OFFLINE}};
    getErrorStorage().sendError(error);
  }
  return false;
}
#endif

EncoderBoard::EncoderBoard(ConnectionInterface &interface) :
  m_parseTimeout(3000),
  m_updateRequest(false),
  m_azimuthSpeedQueue(),
  m_elevationSpeedQueue(),
  m_azimuthImmediateSpeed(0),
  m_elevationImmediateSpeed(0),
  m_azimuthMaximumIntervalSpeed(0),
  m_elevationMaximumIntervalSpeed(0),
  m_azimuth(0),
  m_elevation(0),
  m_previousAzimuth(0),
  m_previousElevation(0),
  m_interface(interface),
  m_azimuthAxisPosition(theodolite_messages_AxisPosition_UNKNOWN_POSITION),
  m_elevationAxisPosition(theodolite_messages_AxisPosition_UNKNOWN_POSITION),
  m_inclinometerXMills(0),
  m_inclinometerYMills(0),
  m_inclinometerXADC(0),
  m_inclinometerYADC(0),
  m_divider(INTERVAL_SPEED_CALC_DIVIDER),
  m_dataReliable(false)
{
  m_parseTimeout.restart();
}

int32_t EncoderBoard::getAzimuthImmediateSpeed()
{
  return m_azimuthImmediateSpeed;
}

int32_t EncoderBoard::getElevationImmediateSpeed()
{
  return m_elevationImmediateSpeed;
}

bool EncoderBoard::isAzimuthAngleOverloaded()
{
  return isAngleOverloaded(m_azimuthImmediateSpeed);
}
bool EncoderBoard::isElevationAngleOverloaded()
{
  return isAngleOverloaded(m_elevationImmediateSpeed);
}
uint32_t EncoderBoard::getAzimuthMaximumIntervalSpeed()
{
  return m_azimuthMaximumIntervalSpeed;
}
uint32_t EncoderBoard::getElevationMaximumIntervalSpeed()
{
  return m_elevationMaximumIntervalSpeed;
}

int32_t EncoderBoard::getAzimuth()
{
  return m_azimuth;
}

int32_t EncoderBoard::getElevation()
{
  return m_elevation;
}

int32_t EncoderBoard::getInclinometerMillSecValue(int32_t encoderCode)
{
#ifndef NEW_ENCODER_BOARD
  static const int32_t ENCODER_MILL_SEC_MIN_VALUE = 256000;
#else
  static const int32_t ENCODER_MILL_SEC_MIN_VALUE = 4096000;
#endif
  static const int32_t ENCODER_MIIL_SEC_PER_CODE = 125;
  return encoderCode * ENCODER_MIIL_SEC_PER_CODE - ENCODER_MILL_SEC_MIN_VALUE;
}

uint16_t EncoderBoard::getInclinometerXADC()
{
  return m_inclinometerXADC;
}

uint16_t EncoderBoard::getInclinometerYADC()
{
  return m_inclinometerYADC;
}

int32_t EncoderBoard::getInclinometerXMills()
{
  return m_inclinometerXMills;
}

int32_t EncoderBoard::getInclinometerYMills()
{
  return m_inclinometerYMills;
}

//static uint_least8_t EncoderBoard::calculateEncoderCodeCrc(RingBuffer *buffer,
//                                                           uint16_t lenght)
//{
//  uint_least8_t crc = 0;
//  for (uint16_t i = 0; i < lenght; i++)
//  {
//    crc = static_cast<uint_least8_t>(crc
//    ^ showSymbolFromRingBuffer(i, buffer));
//  }
//  return crc;
//}

bool EncoderBoard::isDataReliable()
{
  return m_dataReliable;
}

void EncoderBoard::setUpdateRequest()
{
  m_updateRequest = true;
}

bool EncoderBoard::isAngleCorrect(int32_t angle)
{
  if (angle >= 0 && angle <= MAX_ANGLE_VALUE)
    return true;

  return false;
}

bool EncoderBoard::parseEncoderCode(Data<uint_least8_t> &data)
{
  int32_t parserCursor = 0;
  const int32_t azimuth = (static_cast<int32_t>(data.m_data[parserCursor]))
  | ((static_cast<int32_t>(data.m_data[parserCursor + 1])) << 8)
  | ((static_cast<int32_t>(data.m_data[parserCursor + 2])) << 16);
  m_azimuth = azimuth;
  parserCursor += 3;
  const int32_t elevation = (static_cast<int32_t>(data.m_data[parserCursor]))
  | ((static_cast<int32_t>(data.m_data[parserCursor + 1])) << 8)
  | ((static_cast<int32_t>(data.m_data[parserCursor + 2])) << 16);
  m_elevation = elevation;
  parserCursor += 3;
  const uint16_t inclinometerX =
  static_cast<uint16_t>(((uint16_t)data.m_data[parserCursor])
  | ((static_cast<uint16_t>(data.m_data[parserCursor + 1]))
  << 8));
  m_inclinometerXADC = inclinometerX;
  m_inclinometerXMills = getInclinometerMillSecValue(inclinometerX);
  parserCursor += 2;
  const uint16_t inclinometerY =
  static_cast<uint16_t>((static_cast<uint16_t>(data.m_data[parserCursor]))
  | ((static_cast<uint16_t>(data.m_data[parserCursor + 1]))
  << 8));
  m_inclinometerYADC = inclinometerY;
  m_inclinometerYMills = getInclinometerMillSecValue(inclinometerY);
  parserCursor += 2;
  m_azimuthAxisPosition =
  static_cast<theodolite_messages_AxisPosition>(data.m_data[parserCursor++]);
  m_elevationAxisPosition =
  static_cast<theodolite_messages_AxisPosition>(data.m_data[parserCursor]);
  return true;
}

bool EncoderBoard::parseEncoderCode(QueueInterface<uint_least8_t> *buffer)
{
  bool status = true;
  uint_least8_t codeArray[ENCODER_BOARD_CODE_LENGTH - 1]; //whithout marker
  for (uint16_t i = 0; i < ENCODER_BOARD_CODE_LENGTH - 1; i++)
  {
    codeArray[i] = buffer->get();
  }
  int32_t parserCursor = 0;
  const int32_t azimuth = (static_cast<int32_t>(codeArray[parserCursor]))
  | ((static_cast<int32_t>(codeArray[parserCursor + 1])) << 8)
  | ((static_cast<int32_t>(codeArray[parserCursor + 2])) << 16);

  if (isAngleCorrect(azimuth))
    m_azimuth = azimuth;
  else
    status = false;

  parserCursor += 3;
  const int32_t elevation = (static_cast<int32_t>(codeArray[parserCursor]))
  | ((static_cast<int32_t>(codeArray[parserCursor + 1])) << 8)
  | ((static_cast<int32_t>(codeArray[parserCursor + 2])) << 16);

  if (isAngleCorrect(elevation))
    m_elevation = elevation;
  else
    status = false;

  parserCursor += 3;
  const uint16_t inclinometerX =
  static_cast<uint16_t>(((uint16_t)codeArray[parserCursor])
  | ((static_cast<uint16_t>(codeArray[parserCursor + 1])) << 8));
  m_inclinometerXADC = inclinometerX;
  m_inclinometerXMills = getInclinometerMillSecValue(inclinometerX);
  parserCursor += 2;
  const uint16_t inclinometerY =
  static_cast<uint16_t>((static_cast<uint16_t>(codeArray[parserCursor]))
  | ((static_cast<uint16_t>(codeArray[parserCursor + 1])) << 8));
  m_inclinometerYADC = inclinometerY;
  m_inclinometerYMills = getInclinometerMillSecValue(inclinometerY);
  return status;
}

