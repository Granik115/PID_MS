/*
 * soft_uart_tst.cpp
 *
 *      Author: mmalenko
 * (c) 2016 NIC SPbETU
 */

#include "soft_uart_tst.h"
#include <theodolite/logic/software_uart/sw_uart.h>
#include <gtest/gtest.h>

using ::testing::InSequence;

TEST(SoftUartTest,SendByte)
{
  GpioPinMock pin;
  SoftUart uart = SoftUart(&pin);
  uint_least8_t sendingByte = 0x34; //0b00110100
  uart.send(sendingByte);
  {
    InSequence dummy;
    EXPECT_CALL(pin, setLow());  //Start bit
    EXPECT_CALL(pin, setLow());//0
    EXPECT_CALL(pin, setLow());//0
    EXPECT_CALL(pin, setHigh());//1
    EXPECT_CALL(pin, setLow());//0
    EXPECT_CALL(pin, setHigh());//1
    EXPECT_CALL(pin, setHigh());//1
    EXPECT_CALL(pin, setLow());//0
    EXPECT_CALL(pin, setLow());//0
    EXPECT_CALL(pin, setHigh());//Stop bit
  }
  for (uint16_t i = 0; i < 20; i++)
  {
    uart.isrCallFunction();
  }

}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
