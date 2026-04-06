/*
 * camera_tst.cpp
 *
 *      Author: mmalenko
 * (c) 2016 NIC SPbETU
 */

#include "camera_tst.h"
#include "theodolite/logic/camera_protocol/camera.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::InSequence;

static uint_least8_t cameraCommandCode[CAMERA_COMMAND_SIZE] =
{0xA2, 0x43, 0x56, 0x34, 0x12, 0x4, 0x0};

TEST(CameraTest, SendCommand)
{
  CameraConnectorMock connector;
  Camera camera = Camera(&connector);
  {
    InSequence dummy;
    for (uint16_t i = 0; i < CAMERA_COMMAND_SIZE; i++)
    {
      EXPECT_CALL(connector, send(cameraCommandCode[i]));
    }
  }
  camera.setExposition(0x123456);
  camera.setExpoType(CameraExpositionType_MANUAL);
  camera.setSyncType(CameraSyncType_ORIGIN);
  camera.setFrequency(CameraFreq_FREQ_2);
  camera.setGain(CameraGain_GAIN_4);
  camera.applyChanges();
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
