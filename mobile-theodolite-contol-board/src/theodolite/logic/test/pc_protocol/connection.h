/*
 * connection.h
 *
 *  Created on: 4 окт. 2018 г.
 *      Author: maxim
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "firmware/interfaces/interfaces.h"

class ConnectionMock:
  public ConnectionInterface
{
  FastQueue<uint_least8_t> rxBuffer;
  public:
  bool sendPendingByte()
  {
    return false;
  }
  ConnectionMock()
  {

  }
  bool send(Data<uint_least8_t> *data)
  {
    return false;
  }
  bool send(uint_least8_t byte)
  {
    return false;
  }
  FastQueue<uint_least8_t>* getRxQueue()
  {
    return &rxBuffer;
  }
  RingBuffer* getTxQueue()
  {
    return NULL;
  }
  uint16_t receive()
  {
    return 0;
  }
  uint16_t receive(Data<uint_least8_t> *data)
  {
    return 0;
  }
  bool isBusy()
  {
    return false;
  }
  bool isPeripheralOk()
  {
    return true;
  }
};

#endif /* CONNECTION_H_ */
