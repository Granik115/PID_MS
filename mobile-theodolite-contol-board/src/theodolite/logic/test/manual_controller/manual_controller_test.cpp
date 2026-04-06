/*
 * manual_controller_test.cpp
 *
 *  Created on: 18 авг. 2017 г.
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <theodolite/logic/manual_controller/manual_controller.h>
#include <theodolite/logic/test/move_controller/move_controller_mock.h>

using ::testing::InSequence;
using ::testing::AtLeast;

class TestPin:
  public GpioPinInterface
{
  bool level;
  public:
  TestPin()
  {
    level = false;
  }
  void setHigh()
  {
    level = true;
  }
  void setLow()
  {
    level = false;
  }
  void setLevel(bool level)
  {
    this->level = level;
  }
  bool getLevel()
  {
    return level;
  }
};

class ButtonMock:
  public ButtonInterface
{
public:
  MOCK_METHOD0(update,void());
};

TEST(ManualControllerTest, ConnectingTest)
{
  /*
  MoveControllerMock moveController;

  ButtonMock azimuthUpButton;
  ButtonMock azimuthDownButton;
  ButtonMock elevationUpButton;
  ButtonMock elevationDownButton;
  GpioPinInterface *connectControllerPin = new TestPin();
  ManualController manualController = ManualController(moveController,
  azimuthUpButton, azimuthDownButton, elevationUpButton,
  elevationDownButton, connectControllerPin);

  connectControllerPin->setLow();
  EXPECT_CALL(azimuthUpButton, update()).Times(0);
  EXPECT_CALL(azimuthDownButton, update()).Times(0);
  EXPECT_CALL(elevationUpButton, update()).Times(0);
  EXPECT_CALL(elevationDownButton, update()).Times(0);
  manualController.update();

  connectControllerPin->setHigh();
  EXPECT_CALL(azimuthUpButton, update()).Times(AtLeast(1));
  EXPECT_CALL(azimuthDownButton, update()).Times(AtLeast(1));
  EXPECT_CALL(elevationUpButton, update()).Times(AtLeast(1));
  EXPECT_CALL(elevationDownButton, update()).Times(AtLeast(1));
  manualController.update();

  putSymbolToRingBuffer(static_cast<uint8_t>(CONTROLLER_CONNECT),
  getControlsRingBuffer());
  putSymbolToRingBuffer(static_cast<uint8_t>(AZIMUTH_CLOCKWISE_PRESSED),
  getControlsRingBuffer());
  putSymbolToRingBuffer(static_cast<uint8_t>(AZIMUTH_CLOCKWISE_RELEASED),
  getControlsRingBuffer());
  putSymbolToRingBuffer(
  static_cast<uint8_t>(AZIMUTH_COUNTER_CLOCKWISE_PRESSED),
  getControlsRingBuffer());
  putSymbolToRingBuffer(
  static_cast<uint8_t>(AZIMUTH_COUNTER_CLOCKWISE_RELEASED),
  getControlsRingBuffer());
  putSymbolToRingBuffer(static_cast<uint8_t>(ELEVATION_UP_PRESSED),
  getControlsRingBuffer());
  putSymbolToRingBuffer(static_cast<uint8_t>(ELEVATION_UP_RELEASED),
  getControlsRingBuffer());
  putSymbolToRingBuffer(static_cast<uint8_t>(ELEVATION_DOWN_PRESSED),
  getControlsRingBuffer());
  putSymbolToRingBuffer(static_cast<uint8_t>(ELEVATION_DOWN_RELEASED),
  getControlsRingBuffer());
  putSymbolToRingBuffer(static_cast<uint8_t>(CONTROLLER_DISCONNECT),
  getControlsRingBuffer());

  {
    InSequence dummy;
    EXPECT_CALL(moveController, ignoreExplicitAngle(true)); //CONTROLLER_CONNECT
    EXPECT_CALL(moveController, stopAzimuthAxis());//CONTROLLER_CONNECT
    EXPECT_CALL(moveController, stopElevationAxis());//CONTROLLER_CONNECT
    EXPECT_CALL(moveController, rotateAzimuthAxis(CLOCKWISE));//AZIMUTH_CLOCKWISE_PRESSED
    EXPECT_CALL(moveController, stopAzimuthAxis());//AZIMUTH_CLOCKWISE_RELEASED
    EXPECT_CALL(moveController, rotateAzimuthAxis(COUNTERCLOCKWISE));//AZIMUTH_COUNTER_CLOCKWISE_PRESSED
    EXPECT_CALL(moveController, stopAzimuthAxis());//AZIMUTH_CLOCKWISE_RELEASED
    EXPECT_CALL(moveController, rotateElevationAxis(COUNTERCLOCKWISE));//ELEVATION_UP_PRESSED
    EXPECT_CALL(moveController, stopElevationAxis());////ELEVATION_UP_RELEASED
    EXPECT_CALL(moveController, rotateElevationAxis(CLOCKWISE));//ELEVATION_DOWN_PRESSED
    EXPECT_CALL(moveController, stopElevationAxis());////ELEVATION_UP_RELEASED
    EXPECT_CALL(moveController, ignoreExplicitAngle(false));//CONTROLLER_DISCONNECT
  }

  while (getRingBufferCount(getControlsRingBuffer()))
  {
    manualController.update();
  }*/
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
