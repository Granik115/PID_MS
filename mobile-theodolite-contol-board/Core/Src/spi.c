/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    spi.c
 * @brief   This file provides code for the configuration
 *          of the SPI instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */
#ifdef MKTI_OS
#include "cmsis_os2.h"
#endif
#include "theodolite/peripheral/ethernetif/spi_mac_driver/nic.h"
#define ENC28J60_SPI SPI3
/* USER CODE END 0 */

/* SPI3 init function */
void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */
  LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_0);
  LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_5);
  /* USER CODE END SPI3_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**SPI3 GPIO Configuration
  PB3   ------> SPI3_SCK
  PB4   ------> SPI3_MISO
  PB5   ------> SPI3_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* SPI3 DMA Init */

  /* SPI3_RX Init */
  LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_0, LL_DMA_CHANNEL_0);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_0, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_0, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_0, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_0, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_0, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_0, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_0, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_0);

  /* SPI3_TX Init */
  LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_5, LL_DMA_CHANNEL_0);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_5, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_5, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_5, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_5, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_5, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_5, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_5, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_5);

  /* USER CODE BEGIN SPI3_Init 1 */
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_5, LL_SPI_DMA_GetRegAddr(SPI3));
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_0, LL_SPI_DMA_GetRegAddr(SPI3));
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_5);
  /* USER CODE END SPI3_Init 1 */
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI3, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI3, LL_SPI_PROTOCOL_MOTOROLA);
  /* USER CODE BEGIN SPI3_Init 2 */
  LL_SPI_Enable(SPI3);
  /* USER CODE END SPI3_Init 2 */

}

/* USER CODE BEGIN 1 */
error_t extIntInit(void)
{
  return -1;
}

void extIntEnc28J60EnableIrq(void)
{
  NVIC_EnableIRQ(EXTI9_5_IRQn);
}

error_t spiSetMode(uint16_t mode)
{
  return -1;
}

error_t spiSetBitrate(uint16_t bitrate)
{
  return -1;
}

static inline uint8_t spiTransfer_priv(SPI_TypeDef *spi, uint8_t data)
{
  LL_SPI_TransmitData8(spi, data);
  while (!LL_SPI_IsActiveFlag_RXNE(spi))
  //while (LL_SPI_IsActiveFlag_BSY(SPI1))
  {
  }
  return LL_SPI_ReceiveData8(spi);
}

void extIntEnc28J60DisableIrq(void)
{
  NVIC_DisableIRQ(EXTI9_5_IRQn);
}

#ifdef MKTI_OS
osSemaphoreId_t dmaSpiSemaphore;
#endif

error_t spiEnc28j60Init(void)
{
  MX_SPI3_Init();
#ifdef MKTI_OS
  dmaSpiSemaphore = osSemaphoreNew(1, 0, NULL);
  if (dmaSpiSemaphore != NULL)
  {
    return 0;
  }
#endif
  return 0;
}

void spiEnc28j60AssertCs(void)
{
  LL_GPIO_ResetOutputPin(ENC_ETH_CS_GPIO_Port, ENC_ETH_CS_Pin);
}

void spiEnc28j60DeassertCs(void)
{
  LL_GPIO_SetOutputPin(ENC_ETH_CS_GPIO_Port, ENC_ETH_CS_Pin);
}

inline static void sendSpiEnc28J60TxDmaChannel(uint8_t *buffer, uint32_t len)
{
  LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_5);
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_5, (uint32_t)buffer);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_5, len);
  LL_SPI_EnableDMAReq_TX(ENC28J60_SPI);
  LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_5);
  //while (!LL_DMA_IsActiveFlag_TC3(DMA1))
#ifdef MKTI_OS
  {
    osSemaphoreAcquire(dmaSpiSemaphore, osWaitForever);
  }
#endif
  while (LL_SPI_IsActiveFlag_BSY(ENC28J60_SPI))
  {

  }
  LL_DMA_ClearFlag_TC5(DMA1);
  LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_5);
}

inline static void configSpiEnc28J60RxDmaChannel(uint8_t *buffer, uint32_t len)
{
  LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_0);
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_0, (uint32_t)buffer);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, len);
  LL_SPI_EnableDMAReq_RX(ENC28J60_SPI);
  LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
  sendSpiEnc28J60TxDmaChannel(buffer, len);
  while (!LL_DMA_IsActiveFlag_TC0(DMA1))
  {

  }
  LL_DMA_ClearFlag_TC0(DMA1);
  LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_0);
  LL_SPI_DisableDMAReq_RX(ENC28J60_SPI);
}

void spiEnc28J60TransferDma(uint8_t *buffer, uint32_t len, SpiDirection direction)
{
  if (len == 0)
    return;
  if (direction == RX_SPI_DIRECTION) //RX
  {
    configSpiEnc28J60RxDmaChannel(buffer, len);
  }
  else if (direction == TX_SPI_DIRECTION) //TX
  {
    sendSpiEnc28J60TxDmaChannel(buffer, len);
    while (LL_SPI_IsActiveFlag_RXNE(ENC28J60_SPI))
    {
      LL_SPI_ReceiveData8(ENC28J60_SPI);
    }
  }
}

uint8_t spiEnc28j60Transfer(uint8_t data)
{
  return spiTransfer_priv(ENC28J60_SPI, data);
}

const ExtIntDriver ext1driver =
{.init = extIntInit, .enableIrq = extIntEnc28J60EnableIrq, .disableIrq =
extIntEnc28J60DisableIrq};

const SpiDriver spi1driver =
{.init = spiEnc28j60Init, .setMode = spiSetMode, .setBitrate = spiSetBitrate,
  .assertCs = spiEnc28j60AssertCs, .deassertCs = spiEnc28j60DeassertCs, .transfer =
  spiEnc28j60Transfer, .spiTransferDma = &spiEnc28J60TransferDma};

/* USER CODE END 1 */
