/*
 * modbus_inclinometer.cpp
 *
 *  Created on: Jan 17, 2025
 *      Author: mvm
 */

#include "encoder_board/digital_inclinometer/modbus.h"

void initDigitalInclinometer(uint8_t devId, uint16_t firstReg, uint16_t quantityOfReg);

static void sendData(uint8_t *data, uint8_t size)
{
  for (int i = 0; i < size; i++)
  {
    //putSymbolToRingBuffer(data[i], &commonUart.outBuffer);
  }
}

static int16_t receiveData(void)
{
  return 0; //getSymbolFromRingBuffer(&commonUart.inBuffer);
}

void initDigitalInclinometer(uint8_t devId, uint16_t firstReg, uint16_t quantityOfReg)
{
  modbusInit(sendData, receiveData);
  ModbusPackage pckg = {};
  pckg.id = devId;
  pckg.func = READ_HOLDING_REGISTERS;
  pckg.addrFirstReg = firstReg;
  pckg.quantityOfReg = quantityOfReg;
  modbusSendCmd(&pckg);
}
