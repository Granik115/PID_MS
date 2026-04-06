/*
 * tcp_connection_interface.h
 *
 *  Created on: Jan 10, 2025
 *      Author: mvm
 */

#ifndef THEODOLITE_LOGIC_TCP_SERVER_TCP_CONNECTION_INTERFACE_H_
#define THEODOLITE_LOGIC_TCP_SERVER_TCP_CONNECTION_INTERFACE_H_

#include <firmware/interfaces/interfaces.h>
#include "firmware/queue/fast_queue.h"
#include "cmsis_os2.h"

const uint8_t SOCKETS_MAX_COUNT = 5;

class TcpConnection:
  public ConnectionInterface
{
  static const uint32_t QUEUE_MASK = 0x1FF;
  FastQueue<uint_least8_t, QUEUE_MASK> m_rxQueue;
  osMessageQueueId_t m_txQueue;
  int m_sock;
  public:
  void setSocket(int sock);
  int getSocket();
  bool sendPendingByte();
  TcpConnection();
  bool send(Data<uint_least8_t> *data);
  bool send(uint_least8_t byte);
  QueueInterface<uint_least8_t>* getRxQueue();
  osMessageQueueId_t getTxQueue();
  uint16_t receive();
  uint16_t receive(Data<uint_least8_t> *data);
  bool isBusy();
  bool isPeripheralOk();
  void clearData();
};

Data<ConnectionInterface> getTcpConnectionsPool();
void deallocData(Data<uint8_t> *data);

#endif /* THEODOLITE_LOGIC_TCP_SERVER_TCP_CONNECTION_INTERFACE_H_ */
