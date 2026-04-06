/*
 * test_interface.h
 *
 *  Created on: 11 апр. 2019 г.
 *      Author: mmalenko
 */

#ifndef SRC_THEODOLITE_LOGIC_TEST_TEST_INTERFACE_TEST_INTERFACE_H_
#define SRC_THEODOLITE_LOGIC_TEST_TEST_INTERFACE_TEST_INTERFACE_H_

#include <firmware/interfaces/interfaces.h>

class TestInterface:
  public ConnectionInterface
{
  FastQueue<uint_least8_t> rxBuffer;
  FastQueue<uint_least8_t> txBuffer;
  public:

  TestInterface()
  {
  }

  bool isPeripheralOk()
  {
    return true;
  }

  bool sendPendingByte()
  {
    return true;
  }

  bool send(uint_least8_t byte)
  {
    return true;
  }
  bool send(Data<uint_least8_t> *data)
  {
    for (uint16_t i = 0; i < data->m_size; i++)
    {
      txBuffer.put(*(data->m_data + i));
    }
    return true;
  }
  uint16_t receive()
  {
    return 0;
  }

  uint16_t receive(Data<uint_least8_t> *data)
  {
    uint16_t receiveDataSize = getUartLinkData(&rxBuffer, data, true, 0xC0);
    return receiveDataSize;
  }
  FastQueue<uint_least8_t> *getTxQueue()
  {
    return &txBuffer;
  }
  FastQueue<uint_least8_t> *getRxQueue()
  {
    return &rxBuffer;
  }

  bool isBusy()
  {
    return false;
  }

};

#endif /* SRC_THEODOLITE_LOGIC_TEST_TEST_INTERFACE_TEST_INTERFACE_H_ */
