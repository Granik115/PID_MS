/*
 * move_controller_mock.h
 *
 *  Created on: 18 авг. 2017 г.
 *      Author: mmalenko
 *  (c) 2017 NIC SPbETU
 */

#ifndef MOVE_CONTROLLER_MOCK_H_
#define MOVE_CONTROLLER_MOCK_H_

#include <gmock/gmock.h>
#include <theodolite/logic/move_controller/move_controller_interface.h>

class MoveControllerMock:
  public MoveControllerInterface
{
public:
  MOCK_METHOD2(setExplicitAngle,void(uint32_t azimuth, uint32_t elevation));
  MOCK_METHOD1(rotateAzimuthAxis,void(RotationDirection));
  MOCK_METHOD0(rotateAzimuthAxisClockwise90,void());
  MOCK_METHOD0(rotateAzimuthAxisClockwise180,void());
  MOCK_METHOD1(rotateElevationAxis,void(RotationDirection));
  MOCK_METHOD0(stopAzimuthAxis,void());
  MOCK_METHOD0(stopElevationAxis,void());
  MOCK_METHOD1(ignoreExplicitAngle,void(bool));
  MOCK_METHOD0(getAzimuthAxisMover,axis_mover_interface::AxisMoverInterface&());
  MOCK_METHOD0(getElevationAxisMover,axis_mover_interface::AxisMoverInterface&());
};

#endif /* MOVE_CONTROLLER_MOCK_H_ */
