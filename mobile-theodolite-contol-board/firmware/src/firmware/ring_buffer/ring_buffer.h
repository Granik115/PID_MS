/*
 * ring_buffer.h
 *
 (c) 2015 NIC SPbETU
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include "stdint.h"

#define BUF_SIZE    256
#define BUF_MASK    (BUF_SIZE-1)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  uint_least8_t buffer[BUF_SIZE];
  uint16_t idxIn;
  uint16_t idxOut;
} RingBuffer;

void removeSymbolsFromRingBuffer(uint_least8_t numberOfSymbols,
                                 RingBuffer* buf);
uint16_t getRingBufferCount(RingBuffer*);
void clearRingBuffer(RingBuffer*);
void putSymbolToRingBuffer(uint_least8_t symbol, RingBuffer* buf);
uint_least8_t getSymbolFromRingBuffer(RingBuffer*);
int16_t showSymbolFromRingBuffer(uint16_t, RingBuffer*);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H_ */
