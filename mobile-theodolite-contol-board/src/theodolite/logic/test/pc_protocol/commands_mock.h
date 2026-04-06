/*
 * commands_mock.h
 *
 *      Author: mmalenko
 *  (c) 2016 NIC SPbETU
 */

#ifndef LOGIC_PC_PROTOCOL_COMMANDS_MOCK_H_
#define LOGIC_PC_PROTOCOL_COMMANDS_MOCK_H_

#include <theodolite/logic/pc_protocol/commands_interface.h>
#include "firmware/interfaces/interfaces.h"
#include <firmware/link_level/uart_link.h>
#include <gmock/gmock.h>

class MockCommands:
  public CommandsInterface
{
public:
  MOCK_METHOD3(rotateToCoordinatesInEncoderValues,void(uint32_t azimuth, uint32_t elevation,bool));
  MOCK_METHOD3(rotateToCoordinatesInMills,void(uint32_t azimuth, uint32_t elevation,bool));
  MOCK_METHOD1(setCameraExposure,void(uint32_t exposure));
  MOCK_METHOD0(getState,SystemState(void));
  MOCK_METHOD0(applyCameraChanges,void(void));
  MOCK_METHOD2(sendCameraRawCommand,bool(uint_least8_t *buffer, uint16_t len));
  MOCK_METHOD1(setCameraGain,void(uint32_t gain));
  MOCK_METHOD1(setCameraFrequency,void(CameraFreq frequency));
  MOCK_METHOD1(setCameraExpoType,void(CameraExpositionType expoType));
  MOCK_METHOD1(setCameraSyncType,void(CameraSyncType syncType));
  MOCK_METHOD1(setAzimuthPidCoeff,void(theodolite_messages_PidRegulatorCoeff));
  MOCK_METHOD1(setElevationPidCoeff,void(theodolite_messages_PidRegulatorCoeff));
  MOCK_METHOD0(getAzimuthMoveProcessor, axis_mover_interface::AxisMoverInterface&(()));
  MOCK_METHOD0(getElevationMoveProcessor, axis_mover_interface::AxisMoverInterface&(()));
  MOCK_METHOD0(getManualControllerEventQueue, Queue<ControllersEvent>*(()));
  MOCK_METHOD0(getTestingQueue,Queue<Test>*(()));
  MOCK_METHOD0(setTestMode,void(()));
  MOCK_METHOD0(updateTest,void(()));
};

class TestInterface:
  public ConnectionInterface
{

  FastQueue<uint_least8_t> rxBuffer;
  FastQueue<uint_least8_t> txBuffer;
  public:

  TestInterface()
  {
  }

  bool isPeripheralOk()
  {
    return true;
  }

  bool sendPendingByte()
  {
    return true;
  }

  bool send(uint_least8_t byte)
  {
    return true;
  }
  bool send(Data<uint_least8_t> *data)
  {
    for (uint16_t i = 0; i < data->m_size; i++)
    {
      txBuffer.put(*(data->m_data + i));
    }
    return true;
  }
  uint16_t receive()
  {
    return 0;
  }

  uint16_t receive(Data<uint_least8_t> *data)
  {
    uint16_t receiveDataSize = getUartLinkData(&rxBuffer, data, true, 0xC0);
    return receiveDataSize;
  }
  FastQueue<uint_least8_t> *getTxQueue()
  {
    return &txBuffer;
  }
  FastQueue<uint_least8_t> *getRxQueue()
  {
    return &rxBuffer;
  }

  bool isBusy()
  {
    return false;
  }

};

#endif /* LOGIC_PC_PROTOCOL_COMMANDS_MOCK_H_ */
