/*
 * inclinometer_mock.h
 *
 *  Created on: 11 апр. 2019 г.
 *      Author: mmalenko
 *  (c) 2019 NIC SPbETU
 */

#ifndef SRC_THEODOLITE_LOGIC_TEST_ENCODER_BOARD_PROTOCOL_INCLINOMETER_MOCK_H_
#define SRC_THEODOLITE_LOGIC_TEST_ENCODER_BOARD_PROTOCOL_INCLINOMETER_MOCK_H_

#include <theodolite/logic/encoder_board/inclinometer/inclinometer_interface.h>
#include <gmock/gmock.h>

class InclinometerMock:
  public InclinometerInterface
{
public:
  MOCK_METHOD0(update,void());
  MOCK_METHOD0(getX,int32_t());
  MOCK_METHOD0(getY,int32_t());
  MOCK_METHOD0(getXSec,int32_t());
  MOCK_METHOD0(getYSec,int32_t());
};

#endif /* SRC_THEODOLITE_LOGIC_TEST_ENCODER_BOARD_PROTOCOL_INCLINOMETER_MOCK_H_ */
