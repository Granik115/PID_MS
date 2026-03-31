/*
 * net.h
 *
 *  Created on: Feb 26, 2021
 *      Author: maxim
 */

#ifndef _NET_H
#define _NET_H

//Forward declaration of NetInterface structure
struct _NetInterface;
#define NetInterface struct _NetInterface

#define MAC_ADDR_FILTER_SIZE 12
#define NIC_CONTEXT_SIZE   4

#include "stdbool.h"
#include "stdint.h"
#include "net_buf.h"
#include "nic.h"
#include "os_port.h"
#include "error.h"
/**
 * @brief MAC address
 **/

typedef struct
{
  union
  {
    uint8_t b[6];
    uint16_t w[3];
  };
} MacAddr;

/**
 * @brief MAC filter table entry
 **/

typedef struct
{
  MacAddr addr;    ///<MAC address
  uint32_t refCount; ///<Reference count for the current entry
  bool addFlag;
  bool deleteFlag;
} MacFilterEntry;

/**
 * @brief driver context
 **/

typedef struct
{
  uint16_t next_packet; ///<Next packet in the receive buffer
} Context;

struct _NetInterface
{
  char name[2];
  uint8_t nicContext[NIC_CONTEXT_SIZE];
  MacAddr macAddr;
  MacFilterEntry macAddrFilter[MAC_ADDR_FILTER_SIZE]; ///<MAC filter table
  uint32_t rx_buffer_start;
  uint32_t rx_buffer_stop;
  uint32_t tx_buffer_start;
  uint32_t tx_buffer_stop;
  NicDuplexMode duplexMode;                      ///<Duplex mode
  const SpiDriver *spiDriver;                        ///<Underlying SPI driver
  const ExtIntDriver *extIntDriver;         ///<External interrupt line driver
  bool linkState;                              ///<Link state
  uint32_t linkSpeed;                            ///<Link speed
  bool_t nicEvent;                               ///<A NIC event is pending
  OsEvent nicTxEvent;                                  ///<Network controller TX event
  OsEvent nicRxEvent;                                  ///<Network controller RX event
  OsEvent netEvent;
#ifdef MKTI_OS
	osSemaphoreId_t irq_event_semaphore;
	osSemaphoreId_t tx_ready_semaphore;
	osMessageQueueId_t tx_message_queue;
	osMutexId_t mutex;
#endif
	void *ipStackInterface;
};

#endif
