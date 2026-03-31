///*(c) 2017 NIC SPbETU
// * pc_uart.h
// *
// *  Created on: 18 апр. 2018 г.
// *      Author: mmalenko
// */

#ifndef SRC_THEODOLITE_PERIPHERAL_UART_PC_UART_PC_UART_H_
#define SRC_THEODOLITE_PERIPHERAL_UART_PC_UART_PC_UART_H_

#include <firmware/interfaces/interfaces.h>
#include "firmware/queue/fast_queue.h"

class PcUart:
  public ConnectionInterface
{
  static const uint32_t QUEUE_MASK = 0x1FF;
  FastQueue<uint_least8_t, QUEUE_MASK> rxQueue;
  FastQueue<uint_least8_t, QUEUE_MASK> txQueue;
  static const uint32_t ENCODER_UART_SPEED = 625000;
  public:
  bool sendPendingByte();
  PcUart();
  bool send(Data<uint_least8_t> *data);
  bool send(uint_least8_t byte);
  QueueInterface<uint_least8_t>* getRxQueue();
  QueueInterface<uint_least8_t>* getTxQueue();
  uint16_t receive();
  uint16_t receive(Data<uint_least8_t> *data);
  bool isBusy();
  bool isPeripheralOk()
  {
    return true;
  }
};

void pcUartConfig(uint32_t baudrate);

#endif /* SRC_THEODOLITE_PERIPHERAL_UART_PC_UART_PC_UART_H_ */
