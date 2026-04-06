///*
// * encoder_main.cpp
// *
// *  Created on: 17 мая 2018 г.
// *      Author: mmalenko
// * (c) 2018 NIC SPbETU
// */

#include <theodolite/logic/sys_config/sys_cfg.h>
#include "theodolite/logic/periph_bridge/uart.h"
#include <theodolite/logic/encoder_board/inclinometer/inclinometer_interface.h>
#include <theodolite/logic/encoder_board/indication/indication_interface.h>
#include <theodolite/logic/encoder_board_main/encoder/encoder.h>
#include <theodolite/logic/soft_timer/sw_timer.h>
#include <theodolite/logic/encoder_board_main/encoder_board_protocol/encoder_protocol.h>
#include <theodolite/peripheral/hardware.h>

namespace
{
  const uint32_t DELAY_TO_START = 1000000;

}

extern void updatePeripheralTask();

int main(void)
{
  for (uint32_t i = 0; i < DELAY_TO_START; i++)
  {

  }
  systemConfiguration();
  Timer refreshIndication = Timer(400);
  InclinometerInterface &inclinometer = getAnalogInclinometer();
  IndicationInterface &indication = getIndication();
  EncoderInterface &azimuthEncoder = getAzimithEncoderInterface();
  EncoderInterface &elevationEncoder = getElevationEncoderInterface();
  GpioPin azimuthZeroLed = GpioPin(AZIMUTH_ZERO_LED_GPIO, AZIMUTH_ZERO_LED_GPIO_PIN);
  GpioPin elevationZeroLed = GpioPin(ELEVATION_ZERO_LED_GPIO, ELEVATION_ZERO_LED_GPIO_PIN);
  while (1)
  {
    inclinometer.update();
    if (refreshIndication.expired())
    {
      if (inclinometer.isDataReliable())
      {
        indication.setValue(inclinometer.getXMills() / 1000,
                            inclinometer.getYMills() / 1000);
      }
      else
      {
        indication.setError();
      }
      refreshIndication.restart();
    }
    azimuthZeroLed.setLevel(!azimuthEncoder.isZero());
    elevationZeroLed.setLevel(!elevationEncoder.isZero());
    updatePeripheralTask();
  }
}

EncoderBoardProtocol protocol = EncoderBoardProtocol(
                                                     getAnalogInclinometer(),
                                                     getAzimithEncoderInterface(),
                                                     getElevationEncoderInterface(),
                                                     getMainConnectionInterface());

EncoderBoardProtocolInterface& getProtocolInterface()
{
  return protocol;
}

