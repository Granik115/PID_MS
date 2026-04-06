/*
 * modbus.h
 *
 *  (c) 30 мая 2018 г. apotorochin
 */

#ifndef SRC_INCLUDES_MODBUS_H_
#define SRC_INCLUDES_MODBUS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  READ_HOLDING_REGISTERS = 0x03,
  READ_INPUT_REGISTERS = 0x04,
  WRITE_MULTIPLE_REGISTERS = 0x10
}ModbusCmd;

typedef struct
{
  uint8_t id;
  ModbusCmd func;
  uint16_t addrFirstReg;
  uint16_t quantityOfReg;
  uint16_t *data;
}ModbusPackage;

int8_t modbusSendCmd(ModbusPackage *package);
int8_t modbusReceive(ModbusPackage *package);
void modbusInit(void (*funcTransmit)(uint8_t *data, uint8_t size), int16_t (*funcReceive)(void));

#ifdef __cplusplus
}
#endif

#endif /* SRC_INCLUDES_MODBUS_H_ */
