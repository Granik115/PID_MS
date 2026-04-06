/*
 * tcp_connection_interface.cpp
 *
 *  Created on: Jan 10, 2025
 *      Author: mvm
 */

#include "tcp_server/tcp_connection_interface.h"
#include "string.h"

TcpConnection::TcpConnection()
{
  m_sock = -1;
  m_txQueue = osMessageQueueNew(10, sizeof(Data<uint_least8_t>*), NULL);
}

void TcpConnection::setSocket(int sock)
{
  m_sock = sock;
}

int TcpConnection::getSocket()
{
  return m_sock;
}

bool TcpConnection::sendPendingByte()
{
  if (m_sock < 0)
    return false;
  return false;
}

bool TcpConnection::send(Data<uint_least8_t> *data)
{
  if (m_sock < 0)
    return false;
  uint8_t *buffer = (uint8_t*)new uint8_t[data->m_size];
  memcpy(buffer, data->m_data, data->m_size);
  Data<uint_least8_t> *outData = new Data<uint_least8_t>(buffer, data->m_size);
  if (osMessageQueuePut(m_txQueue, &outData, 0, 0) == osOK)
    return true;
  else
  {
    deallocData(outData);
    return false;
  }
}

bool TcpConnection::send(uint_least8_t byte)
{
  if (m_sock < 0)
    return false;
  return false;
}

osMessageQueueId_t TcpConnection::getTxQueue()
{
  return m_txQueue;
}

QueueInterface<uint_least8_t>* TcpConnection::getRxQueue()
{
  return &m_rxQueue;
}

uint16_t TcpConnection::receive()
{
  return 0;
}
uint16_t TcpConnection::receive(Data<uint_least8_t> *data)
{
  return 0;
}

bool TcpConnection::isBusy()
{
  return false;
}

bool TcpConnection::isPeripheralOk()
{
  return m_sock != -1;
}

void TcpConnection::clearData()
{
  m_rxQueue.clear();
  Data<uint8_t> *data;
  while (osMessageQueueGet(m_txQueue, &data, NULL, 0) == osOK)
    deallocData(data);

}

