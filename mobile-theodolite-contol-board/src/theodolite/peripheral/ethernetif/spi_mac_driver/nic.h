/**
 * @file nic.h
 * @brief Network interface controller abstraction layer
 **/

#ifndef _NIC_H
#define _NIC_H

//Dependencies
#include "error.h"

//Tick interval to handle NIC periodic operations
#ifndef NIC_TICK_INTERVAL
#define NIC_TICK_INTERVAL 1000
#elif (NIC_TICK_INTERVAL < 100)
   #error NIC_TICK_INTERVAL parameter is not valid
#endif

//Size of the NIC driver context
#ifndef NIC_CONTEXT_SIZE
#define NIC_CONTEXT_SIZE 8
#elif (NIC_CONTEXT_SIZE < 1)
   #error NIC_CONTEXT_SIZE parameter is not valid
#endif

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief NIC types
 **/
typedef enum
{
  NIC_TYPE_ETHERNET = 0, ///<Ethernet interface
  NIC_TYPE_PPP = 1, ///<PPP interface
  NIC_TYPE_6LOWPAN = 2  ///<6LoWPAN interface
} NicType;

//SPI abstraction layer
typedef error_t (*SpiInit)(void);
typedef error_t (*SpiSetMode)(uint16_t mode);
typedef error_t (*SpiSetBitrate)(uint16_t bitrate);
typedef void (*SpiAssertCs)(void);
typedef void (*SpiDeassertCs)(void);
typedef uint8_t (*SpiTransfer)(uint8_t data);

//UART abstraction layer
typedef error_t (*UartInit)(void);
typedef void (*UartEnableIrq)(void);
typedef void (*UartDisableIrq)(void);
typedef void (*UartStartTx)(void);

//External interrupt line abstraction layer
typedef error_t (*ExtIntInit)(void);
typedef void (*ExtIntEnableIrq)(void);
typedef void (*ExtIntDisableIrq)(void);

typedef enum
{
  TX_SPI_DIRECTION,
  RX_SPI_DIRECTION
} SpiDirection;

/**
 * @brief SPI driver
 **/

typedef struct
{
  SpiInit init;
  SpiSetMode setMode;
  SpiSetBitrate setBitrate;
  SpiAssertCs assertCs;
  SpiDeassertCs deassertCs;
  SpiTransfer transfer;
  void (*spiTransferDma)(uint8_t *buffer, uint32_t len,
                         SpiDirection direction);
} SpiDriver;

/**
 * @brief External interrupt line driver
 **/

typedef struct
{
  ExtIntInit init;
  ExtIntEnableIrq enableIrq;
  ExtIntDisableIrq disableIrq;
} ExtIntDriver;

/**
 * @brief Link state
 **/

typedef enum
{
  NIC_LINK_STATE_DOWN = 0,
  NIC_LINK_STATE_UP = 1,
  NIC_LINK_STATE_AUTO = 2
} NicLinkState;

/**
 * @brief Duplex mode
 **/

typedef enum
{
  NIC_UNKNOWN_DUPLEX_MODE = 0,
  NIC_HALF_DUPLEX_MODE = 1,
  NIC_FULL_DUPLEX_MODE = 2
} NicDuplexMode;

/**
 * @brief Link speed
 **/

typedef enum
{
  NIC_LINK_SPEED_UNKNOWN = 0,
  NIC_LINK_SPEED_10MBPS = 10000000,
  NIC_LINK_SPEED_100MBPS = 100000000,
  NIC_LINK_SPEED_1GBPS = 1000000000
} NicLinkSpeed;

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
