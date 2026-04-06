/*
 * camera_tst.h
 *
 *      Author: mmalenko
 * (c) 2016 NIC SPbETU
 */
#ifndef LOGIC_CAMERA_PROTOCOL_CAMERA_TST_H_
#define LOGIC_CAMERA_PROTOCOL_CAMERA_TST_H_

#include <theodolite/peripheral/camera_connector/camera_connector.h>
#include <gmock/gmock.h>

class CameraConnectorMock:
  public ConnectionInterface
{
public:
  MOCK_METHOD1(send,bool(uint_least8_t byte));
  MOCK_METHOD1(send,bool(Data *data));
  MOCK_METHOD0(getRxRingBuffer,RingBuffer*());
  MOCK_METHOD0(receive,uint16_t());
  MOCK_METHOD1(receive,uint16_t(Data *data));
  MOCK_METHOD0(isBusy,bool());
  MOCK_METHOD0(isPeripheralOk,bool());
};

#endif /* LOGIC_CAMERA_PROTOCOL_CAMERA_TST_H_ */
