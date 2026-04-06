/*
 * axis_mover_test.cpp
 *
 *  Created on: 8 авг. 2017 г.
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#ifndef AXIS_MOVER_TEST_CPP_
#define AXIS_MOVER_TEST_CPP_

#include "stdlib.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(AxisMoverTest, Initialization)
{
  /*
  EncoderBoardMock encoderBoardMock;
  PidRegulator regulator = PidRegulator(AZ_KP_COEFF, AZ_KI_COEFF,
  AZ_KD_COEFF, AZ_KC_COEFF, AZ_OUT_MAX, AZ_OUT_MIN);
  AzimuthBrakePin azimuthBrakePin = AzimuthBrakePin();
  Brake azimuthBrake = Brake(azimuthBrakePin);
  PwmMock motorMock;
  AzimuthAxisMover axisMover = AzimuthAxisMover(encoderBoardMock, regulator, azimuthBrake,motorMock);
//ASSERT_EQ(axisMover.getState(), axis_mover_interface::INITIALIZATION);
  /*Simulate start rotating*/
  /*S*/
//EXPECT_CALL(encoderBoardMock, isBusy()).Times(CAMERA_COMMAND_SIZE).WillRepeatedly(Return(false));*/
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

#endif /* AXIS_MOVER_TEST_CPP_ */
