/*
 * tcp_server.cpp
 *
 *  Created on: Jan 9, 2025
 *      Author: mvm
 */

#include "tcp_connection_interface.h"
#include "lwip.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "firmware/data/data.h"

static void tcpServerThread(void *arg);
static void tcpPoolThread(void *arg);

const int POLL_TIMEOUT = 10;
const uint16_t TCP_LISTEN_PORT = 4001;

typedef struct struct_sock_t
{
  uint16_t port;
  Data<ConnectionInterface> *connections;
} struct_sock;

typedef struct
{
  int sock;
  osMessageQueueId_t rxData;
  osMessageQueueId_t txData;
} SocketConnection;

struct_sock serverSockSettings;

TcpConnection connectionsPool[SOCKETS_MAX_COUNT];

Data<ConnectionInterface> getTcpConnectionsPool()
{
  Data<ConnectionInterface> data = Data<ConnectionInterface>(NULL, 0);
  data.m_data = connectionsPool;
  data.m_size = SOCKETS_MAX_COUNT;
  return data;
}

bool initTcpServers()
{
  serverSockSettings.port = TCP_LISTEN_PORT;
  serverSockSettings.connections = new Data<ConnectionInterface>(connectionsPool, SOCKETS_MAX_COUNT);
  sys_thread_new("tcp_thread", tcpServerThread, (void*)&serverSockSettings, DEFAULT_THREAD_STACKSIZE * 2,
                 osPriorityNormal);
  return true;
}

extern "C"
{
extern uint8_t ip_inited;
}

static void waitforIP()
{
  for (;;)
  {
    if (!ip_inited)
      osDelay(1000);
    else
      return;
  }
}

//---------------------------------------------------------------
static void tcpServerThread(void *arg)
{
  struct_sock *param;
  int sock, accept_sock;
  struct sockaddr_in address, remotehost;
  socklen_t sockaddrsize;
  param = (struct_sock*)arg;
  waitforIP();

  osMessageQueueId_t poolSocketsQueue = osMessageQueueNew(SOCKETS_MAX_COUNT, sizeof(int), NULL);
  for (;;)
  {
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) >= 0)
    {
      address.sin_family = AF_INET;
      address.sin_port = htons(param->port);
      address.sin_addr.s_addr = INADDR_ANY;
      if (bind(sock, (struct sockaddr *)&address, sizeof (address)) == 0)
      {
        listen(sock, SOCKETS_MAX_COUNT);
        sys_thread_new("tcp_pool_thread", tcpPoolThread, poolSocketsQueue, 4000, osPriorityNormal);
        for (;;)
        {
          accept_sock = accept(sock, (struct sockaddr* )&remotehost, (socklen_t* )&sockaddrsize);
          if (accept_sock >= 0)
            osMessageQueuePut(poolSocketsQueue, &accept_sock, 0, osWaitForever);
        }
      }
      else
      {
        close(sock);
        osDelay(1000);
      }
    }
  }
}

void deallocData(Data<uint8_t> *data)
{
  delete data->m_data;
  delete data;
}

//---------------------------------------------------------------
static void tcpPoolThread(void *arg)
{
  osMessageQueueId_t incomingConnectionsQueue = (osMessageQueueId_t)arg;
  struct pollfd fds[SOCKETS_MAX_COUNT] = {};

  for (uint8_t i = 0; i < SOCKETS_MAX_COUNT; i++)
    fds[i].fd = -1;

  for (;;)
  {
    int newSock;
    if (osMessageQueueGet(incomingConnectionsQueue, &newSock, NULL, 0) == osOK)
    {
      uint8_t isAdded = 0;
      for (uint8_t i = 0; i < SOCKETS_MAX_COUNT; i++)
      {
        if (fds[i].fd == -1)
        {
          fds[i].events = POLLIN;
          fds[i].revents = 0;
          fds[i].fd = newSock;
          connectionsPool[i].setSocket(newSock);
          connectionsPool[i].clearData();
          isAdded = 1;
          break;
        }
      }
      if (!isAdded)
        close(newSock);
    }

    int ret = poll(fds, SOCKETS_MAX_COUNT, POLL_TIMEOUT);
    if (ret == -1)
      continue;
    if (ret == 0)
    {
      uint8_t isDataIncoming = 0;
      for (uint32_t i = 0; i < SOCKETS_MAX_COUNT; i++)
        if (osMessageQueueGetCount(connectionsPool[i].getTxQueue()))
          isDataIncoming = 1;
      if (!isDataIncoming)
        continue;
    }

    for (uint32_t i = 0; i < SOCKETS_MAX_COUNT; i++)
    {
      struct pollfd *fd = &fds[i];
      if (fd->fd < 0)
        continue;
      if (fd->revents & POLLIN)
      {
        fd->revents &= ~POLLIN;
        uint8_t dataBuffer[200];
        ssize_t rc = recv(fd->fd, dataBuffer, sizeof(dataBuffer), 0);
        if (rc > 0)
        {
          for (ssize_t bufferIndex = 0; bufferIndex < rc; bufferIndex++)
            connectionsPool[i].getRxQueue()->put(dataBuffer[bufferIndex]);
        }
        else
        {
          assert(fd->fd==connectionsPool[i].getSocket());
          close(fd->fd);
          fd->fd = -1;
          connectionsPool[i].setSocket(fd->fd);
          connectionsPool[i].clearData();
          continue;
        }
      }
      if (fd->revents & POLLOUT)
      {
        fd->revents &= ~POLLOUT;
        Data<uint8_t> *data;
        while (osMessageQueueGet(connectionsPool[i].getTxQueue(), &data, NULL, 0) == osOK)
        {
          ssize_t writted = send(fd->fd, data->m_data, data->m_size, 0);
          assert(writted>0);
          deallocData(data);
        }
        fd->events &= ~POLLOUT;
      }
      else
      {
        Data<uint8_t> *data;
        if (osMessageQueueGet(connectionsPool[i].getTxQueue(), &data, NULL, 0) == osOK)
        {
          uint8_t tmp[300] = {};
          if (data->m_size < sizeof(tmp))
            memcpy(tmp, data->m_data, data->m_size);
          ssize_t writted = send(fd->fd, tmp, data->m_size, 0);
          if (writted < 0)
          {
            fd->events |= POLLOUT;
            if (osMessageQueuePut(connectionsPool[i].getTxQueue(), &data, 0, 0) == osOK)
              continue;
          }
          deallocData(data);
        }
      }

    }
  }
}

