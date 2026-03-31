/*
 * system.c
 *
 *  Created on: 6 мар. 2021 г.
 *      Author: maxim
 */
#include "main.h"
#include "cmsis_os.h"
#include "firmware/ring_buffer/ring_buffer.h"
#include "string.h"
#include "theodolite/peripheral/cpu_board/config/system.h"
#include "stdlib.h"
#include "stdarg.h"
#include "stdio.h"
#include "usbd_cdc_if.h"

osMutexId_t printfMutex;

void* pvPortCalloc(size_t number, size_t xWantedSize)
{
  void *p = pvPortMalloc(xWantedSize * number);
  if (p)
    memset(p, 0, xWantedSize * number);
  return p;
}

/* Define the linked list structure.  This is used to link free blocks in order
 of their memory address. */
typedef struct A_BLOCK_LINK
{
  struct A_BLOCK_LINK *pxNextFreeBlock; /*<< The next free block in the list. */
  size_t xBlockSize; /*<< The size of the free block. */
} BlockLink_t;

void* malloc(size_t xWantedSize)
{
  return pvPortMalloc(xWantedSize);
}

void* realloc(void *ptr, size_t s)
{
  return pvPortRealloc(ptr, s);
}

void* calloc(size_t number, size_t xWantedSize)
{
  return pvPortCalloc(number, xWantedSize);
}

void free(void *pv)
{
  vPortFree(pv);
}

int rand_h()
{
  while (LL_RNG_IsActiveFlag_DRDY(RNG) == RESET)
  {

  }
  return (int)(LL_RNG_ReadRandData32(RNG) & (unsigned) RAND_MAX);
}

/* The size of the structure placed at the beginning of each allocated memory
 block must by correctly byte aligned. */
static const size_t xHeapStructSize = (sizeof(BlockLink_t)
+ ((size_t)( portBYTE_ALIGNMENT - 1)))
& ~((size_t) portBYTE_ALIGNMENT_MASK);

/* Assumes 8bit bytes! */
#define heapBITS_PER_BYTE		( ( size_t ) 8 )

/* Work out the position of the top bit in a size_t variable. */
static const size_t xBlockAllocatedBit = ((size_t)1)
<< ((sizeof(size_t) * heapBITS_PER_BYTE) - 1);

void* pvPortRealloc(void *ptr, size_t s)
{
  uint8_t *puc = (uint8_t*)ptr;
  BlockLink_t *pxLink;
  void *newBlock = NULL;
  size_t blockSize;

  if (ptr == NULL)
  {
    newBlock = pvPortMalloc(s);
  }
  else
  {
    puc -= xHeapStructSize;
    pxLink = (void*)puc;
    blockSize = (pxLink->xBlockSize & (~xBlockAllocatedBit))
    - xHeapStructSize;
    if (s == 0)
    {
      newBlock = NULL;
    }
    else
    {
      if (s < blockSize)
      {
        blockSize = s;
      }
      vTaskSuspendAll();
      {
        newBlock = pvPortMalloc(s);
        if (newBlock != NULL)
        {
          memcpy(newBlock, ptr, blockSize);
          vPortFree(ptr);
        }
      }
      xTaskResumeAll();
    }
  }
  return newBlock;
}

void rebootDevice()
{
  NVIC_SystemReset();
}

void reboot_cb(void *arg)
{
  rebootDevice();
}

void reboot_message()
{
  osTimerId_t timer = osTimerNew(reboot_cb, osTimerOnce, NULL, NULL);
  osTimerStart(timer, 2000);
}

int platform_printf(const char *__restrict message, ...)
{
  osMutexAcquire(printfMutex, osWaitForever);
  volatile char *task_name = pcTaskGetName(NULL);
  va_list list;
  va_start(list, message);
  const uint16_t BUFFER_SIZE = 512;
  int writed_bytes = 0;
  char *buffer = malloc(BUFFER_SIZE);
  if (!buffer)
    goto exit;
  int rc = snprintf(buffer, BUFFER_SIZE, "[%lu][%s] ",
                    (uint32_t)get_uptime_s() / 1000,
                    task_name);
  rc += vsnprintf(buffer + rc, BUFFER_SIZE - rc, message, list);
  va_end(list);
  writed_bytes = _write(0, buffer, rc);
  free(buffer);
  exit:
  osMutexRelease(printfMutex);
  return writed_bytes;
}

uint32_t get_uptime_s()
{
  return osKernelGetTickCount(); //rtpclock() / 1000000;
}
extern uint8_t cpu_idle;
uint8_t get_proc_load()
{
  return 100 - cpu_idle;
}

uint8_t get_max_mem_load()
{
  size_t free_mem = xPortGetMinimumEverFreeHeapSize();
  return 100
  - (free_mem * 100)
  / configTOTAL_HEAP_SIZE;
}

uint8_t get_mem_load()
{
  size_t free_mem = xPortGetFreeHeapSize();
  return 100
  - (free_mem * 100)
  / configTOTAL_HEAP_SIZE;
}

void resetWatchDog()
{
}

void init_debug()
{
  printfMutex = osMutexNew(NULL);
}

#ifdef DEBUG
RingBuffer txBuffer =
{0};
//RingBuffer rxBuffer =
//{ 0 };

RingBuffer* getDebugTxRingBuffer()
{
  return &txBuffer;
}

//RingBuffer* getDebugRxRingBuffer()
//{
//	return &rxBuffer;
//}

int _write(int file, char *ptr, int len)
{
  //int DataIdx;
  if(len<=0)
    return -1;
  if (CDC_Transmit_FS((uint8_t*)ptr, len) != USBD_OK)
    return 0;
  //for (DataIdx = 0; DataIdx < len; DataIdx++)
  //{
  //  putSymbolToRingBuffer(*ptr++, &txBuffer);
  //}
  //if (getRingBufferCount(&txBuffer))
  //{
  //  LL_USART_EnableIT_TXE(USART1);
  //}
  return len;
}
#endif

int __io_putchar(int ch)
{
  return 0;
}

int __io_getchar(void)
{
  return 0;
}

