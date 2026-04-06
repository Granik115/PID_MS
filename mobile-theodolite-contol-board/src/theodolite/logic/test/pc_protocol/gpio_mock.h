///*
//* axis_mover_test.cpp
// *
// *  Created on: 8 авг. 2017 г.
// *      Author: mmalenko
// * (c) 2017 NIC SPbETU
// */
#ifndef LOGIC_SO_UART_TST_H_
#define LOGIC_SO_UART_TST_H_

#include <theodolite/logic/periph_bridge/gpio/gpio.h>
#include <gmock/gmock.h>

class GpioPinMock:
  public GpioPinInterface
{
public:
  MOCK_METHOD0(getLevel,bool());
  MOCK_METHOD0(setHigh,void());
  MOCK_METHOD0(setLow,void());
  MOCK_METHOD1(setLevel,void(bool));
};

#endif /* LOGIC_SOFTWARE_UART_SOFT_UART_TST_H_ */
