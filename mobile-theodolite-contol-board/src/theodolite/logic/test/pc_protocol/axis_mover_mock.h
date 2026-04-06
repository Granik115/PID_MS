/*
 * axis_mover_mock.h
 *
 *  Created on: 13 окт. 2018 г.
 *      Author: maxim
 *(c) 2018 NIC SPbETU
 */

#ifndef SRC_THEODOLITE_LOGIC_TEST_AXIS_MOVER_AXIS_MOVER_MOCK_H_
#define SRC_THEODOLITE_LOGIC_TEST_AXIS_MOVER_AXIS_MOVER_MOCK_H_

#include <theodolite/logic/axis_mover/axis_mover_interface.h>
#include <gmock/gmock.h>

class AxisMoverMock:
  public axis_mover_interface::AxisMoverInterface
{
public:
  MOCK_METHOD0(isBlockAfterRotationMode,bool());
  MOCK_METHOD1(setInertionCoefficient,void(float));
  MOCK_METHOD2(setExplicitAngleCode,void(uint32_t,bool));
  MOCK_METHOD0(setUpdateRequest,void());
  MOCK_METHOD0(isBrake,bool());
  MOCK_METHOD0(getEncoderBoard,EncoderBoardInterface&());
  MOCK_METHOD0(isDriveActivity,bool());
  MOCK_METHOD0(getState,axis_mover_interface::StateType());
  MOCK_METHOD1(rotate,void(axis_mover_interface::AxisMoveDirection));
  MOCK_METHOD0(release,void());
  MOCK_METHOD0(updateOnRequest,void());
  MOCK_METHOD0(updateWithoutRequest,void());
  MOCK_METHOD0(getAxisPosition,theodolite_messages_AxisPosition());
  MOCK_METHOD1(setMotorRampMode,void(RampMode));
  MOCK_METHOD0(getRegulator,PidRegulatorInterface&());
  MOCK_METHOD0(runTest,void());
  MOCK_METHOD0(isLastTestPassed,bool());
};

#endif /* SRC_THEODOLITE_LOGIC_TEST_AXIS_MOVER_AXIS_MOVER_MOCK_H_ */
