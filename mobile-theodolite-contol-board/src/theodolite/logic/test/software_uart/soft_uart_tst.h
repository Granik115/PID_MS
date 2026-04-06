/*
 * soft_uart_tst.h
 *
 *      Author: mmalenko
 *  (c) 2016 NIC SPbETU
 */

#ifndef LOGIC_SOFTWARE_UART_SOFT_UART_TST_H_
#define LOGIC_SOFTWARE_UART_SOFT_UART_TST_H_

#include <theodolite/logic/periph_bridge/gpio/gpio.h>
#include <gmock/gmock.h>

class GpioPinMock:
  public GpioPinInterface
{
public:
  MOCK_METHOD1(setLevel,void(bool));
  MOCK_METHOD0(getLevel,bool());
  MOCK_METHOD0(setHigh,void());
  MOCK_METHOD0(setLow,void());
};

#endif /* LOGIC_SOFTWARE_UART_SOFT_UART_TST_H_ */
