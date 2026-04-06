///*(c) 2018 NIC SPbETU
// * serial_protocol.cpp
// *
// *  Created on: 15 авг. 2018 г.
// *      Author: maxim
// */
#include <stdint.h>
#include <firmware/ring_buffer/ring_buffer.h>
#include "firmware/queue/fast_queue.h"
#include <theodolite/logic/encoder_board/encoder_board.h>
#include <theodolite/logic/pc_protocol/serial_protocol/serial_protocol.h>

const uint_least8_t REQUEST_CMD_LEN = 8;
const uint_least8_t POSITION_REPLY_MARKER = 0x50;
const uint_least8_t POSITION_REPLY_LEN = 12;
const uint_least8_t ROTATE_COORDINATES_ACK_MARKER = 0x51;
const uint_least8_t CAMERA_SETTINGS_ACK_MARKER = 0x52;
const uint_least8_t BLOCKING_STATE_MARKER = 0x53;
const uint_least8_t BLOCKING_STATE_LEN = 4;

const uint_least8_t BLOCKING_MODE_ON = 0xAA;
const uint_least8_t BLOCKING_MODE_OFF = 0x0;

static const uint32_t MAX_ANGLE = 720000;

typedef enum
{
  POSITION_REQUEST_MARKER = 0xA0,
  ROTATE_COORDINATES_MARKER = 0xA1,
  CAMERA_SETTINGS_MARKER = 0xA2,
  BLOCKING_STATE_REQUEST_MARKER = 0xA3
} IncomingCommandMarker;

SerialProtocolProcessor::SerialProtocolProcessor(CommandsInterface &commands, ConnectionInterface &interface,
                                                 GpioPinInterface *receiveCommandLedPin,
                                                 GpioPinInterface *replyCommandLedPin) :
  m_interface(interface), commands(commands)
{
  this->protectTimer = new Timer(300);
  this->receiveCommandLedPin = receiveCommandLedPin;
  this->replyCommandLedPin = replyCommandLedPin;
}

uint_least8_t SerialProtocolProcessor::crcParity(QueueInterface<uint_least8_t> *buff, uint16_t len)
{
  uint_least8_t crc = 0;
  for (uint_least8_t i = 0; i < len; i++)
  {
    crc += buff->show(i);
  }
  return crc % 2;
}

bool SerialProtocolProcessor::sendAck(uint_least8_t ackMarker)
{
  uint_least8_t dataBuf[1] =
  {ackMarker};
  Data<uint_least8_t> data = Data<uint_least8_t>(dataBuf, 1);
  replyCommandLedPin->setLevel(!replyCommandLedPin->getLevel());
  return m_interface.send(&data);
}

bool SerialProtocolProcessor::sendBlockingMode()
{
  uint_least8_t cmd[BLOCKING_STATE_LEN] =
  {BLOCKING_STATE_MARKER, 0, 0, 0};
  cmd[1] = BLOCKING_MODE_ON; //always blocked
  if (commands.getState().azimuthBrake)
  {
    cmd[2] = BLOCKING_MODE_ON;
  }
  if (commands.getState().elevationBrake)
  {
    cmd[3] = BLOCKING_MODE_ON;
  }
  Data<uint_least8_t> data = Data<uint_least8_t>(cmd, BLOCKING_STATE_LEN);
  replyCommandLedPin->setLevel(!replyCommandLedPin->getLevel());
  return m_interface.send(&data);
}

void SerialProtocolProcessor::cleanBuffer()
{
  QueueInterface<uint_least8_t> *rxQueue = m_interface.getRxQueue();
  if (!rxQueue)
    return;
  rxQueue->remove(REQUEST_CMD_LEN - 1);
}

bool SerialProtocolProcessor::verifyRequestZero()
{
  QueueInterface<uint_least8_t> *rxQueue = m_interface.getRxQueue();
  if (!rxQueue)
    return false;
  for (uint16_t i = 1; i < REQUEST_CMD_LEN; i++)
  {
    if (rxQueue->show(i))
    {
      rxQueue->get();
      return false;
    }
  }
  cleanBuffer();
  return true;
}

bool SerialProtocolProcessor::sendState()
{
  const SystemState state = commands.getState();
  uint_least8_t cmd[POSITION_REPLY_LEN];
  cmd[0] = POSITION_REPLY_MARKER;
  const uint32_t azimuth = state.azimuthEncoderValues;
  cmd[1] = azimuth & 0xFF;
  cmd[2] = (azimuth >> 8) & 0xFF;
  cmd[3] = (azimuth >> 16) & 0xFF;
  const uint32_t elevation = state.elevationEncoderValues;
  cmd[4] = elevation & 0xFF;
  cmd[5] = (elevation >> 8) & 0xFF;
  cmd[6] = (elevation >> 16) & 0xFF;
  cmd[7] = state.inclinometerXADCValues & 0xFF;
  cmd[8] = (state.inclinometerXADCValues >> 8) & 0xFF;
  cmd[9] = state.inclinometerYADCValues & 0xFF;
  cmd[10] = (state.inclinometerYADCValues >> 8) & 0xFF;
  cmd[11] = crcMod2(cmd, POSITION_REPLY_LEN - 1);
  Data<uint_least8_t> data = Data<uint_least8_t>(cmd, POSITION_REPLY_LEN);
  replyCommandLedPin->setLevel(!replyCommandLedPin->getLevel());
  return m_interface.send(&data);
}

uint_least8_t SerialProtocolProcessor::crcMod2(uint_least8_t *buff, uint16_t len)
{
  uint_least8_t crc = 0;
  for (uint16_t i = 0; i < len; i++)
  {
    crc = crc ^ buff[i];
  }
  return crc;
}

bool SerialProtocolProcessor::parseIncomingMessage()
{
  QueueInterface<uint_least8_t> *rxQueue = m_interface.getRxQueue();
  if (!rxQueue)
    return false;
  if (rxQueue->getCount() >= REQUEST_CMD_LEN)
  {
    protectTimer->restart();
    const IncomingCommandMarker marker = static_cast<IncomingCommandMarker>(rxQueue->show(0));
    switch (marker)
    {
      case POSITION_REQUEST_MARKER:
        {
        if (verifyRequestZero())
        {
          receiveCommandLedPin->setLevel(!receiveCommandLedPin->getLevel());
          return sendState();
        }
        return false;
      }
      case ROTATE_COORDINATES_MARKER:
        {
        const uint_least8_t autoblockingModeMarker = rxQueue->show(REQUEST_CMD_LEN - 1);
        if (autoblockingModeMarker == 0x00 || autoblockingModeMarker == 0xAA)
        {
          const uint32_t azimuthLow = rxQueue->show(1);

          const uint32_t azimuthMiddle = rxQueue->show(2);
          const uint32_t azimuthHigh = rxQueue->show(3);
          const uint32_t azimuth = azimuthHigh << 16 | azimuthMiddle << 8 | azimuthLow;

          const uint32_t elevationLow = rxQueue->show(4);
          const uint32_t elevationMiddle = rxQueue->show(5);
          const uint32_t elevationHigh = rxQueue->show(6);
          const uint32_t elevation = elevationHigh << 16 | elevationMiddle << 8 | elevationLow;

          if (azimuth <= MAX_ANGLE && elevation <= MAX_ANGLE)
          {
            receiveCommandLedPin->setLevel(!receiveCommandLedPin->getLevel());
            commands.rotateToCoordinatesInEncoderValues(azimuth, elevation, true);
            cleanBuffer();
            return sendAck(ROTATE_COORDINATES_ACK_MARKER);
          }
        }
        rxQueue->remove(1);
        return false;
      }
      case CAMERA_SETTINGS_MARKER:
        {
        uint_least8_t crc = rxQueue->show(REQUEST_CMD_LEN - 1);
        //if (EncoderBoard::calculateEncoderCodeCrc(interface->getRxRingBuffer(), REQUEST_CMD_LEN) == crc)
        uint_least8_t calculatedCrc = crcParity(rxQueue, REQUEST_CMD_LEN - 1);
        if (calculatedCrc == crc)
        {
          receiveCommandLedPin->setLevel(!receiveCommandLedPin->getLevel());
          uint_least8_t cmd[REQUEST_CMD_LEN];
          for (uint_least8_t i = 0; i < REQUEST_CMD_LEN; i++)
          {
            cmd[i] = rxQueue->get();
          }
          commands.sendCameraRawCommand(cmd, REQUEST_CMD_LEN);
          return sendAck(CAMERA_SETTINGS_ACK_MARKER);
        }
        rxQueue->remove(1);
        return false;
      }
      case BLOCKING_STATE_REQUEST_MARKER:
        {
        if (verifyRequestZero())
        {
          receiveCommandLedPin->setLevel(!receiveCommandLedPin->getLevel());
          return sendBlockingMode();
        }
        return false;
      }
      default:
        rxQueue->remove(1);
        return false;
    }
  }
  else if (protectTimer->expired())
  {
    protectTimer->restart();
    rxQueue->clear();
  }
  return false;
}

