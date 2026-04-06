/*
 * ring_buffer.c
 *
 *  Created on: 19 апреля 2014 г.
 *      Author: apotorochin
 (c) 2015 NIC SPbETU
 */
#include "ring_buffer.h"

uint16_t getRingBufferCount(RingBuffer *buf)
{
  return (buf->idxIn - buf->idxOut) & BUF_MASK;
}

void clearRingBuffer(RingBuffer* buf)
{
  buf->idxIn = 0;
  buf->idxOut = 0;
}

void removeSymbolsFromRingBuffer(uint_least8_t numberOfSymbols, RingBuffer* buf)
{
  buf->idxOut += numberOfSymbols;
  buf->idxOut &= BUF_MASK;
}

void putSymbolToRingBuffer(uint_least8_t symbol, RingBuffer* buf)
{
  buf->buffer[buf->idxIn++] = symbol;
  buf->idxIn &= BUF_MASK;
}

uint_least8_t getSymbolFromRingBuffer(RingBuffer *buf)
{
  if (buf->idxOut != buf->idxIn)
  {
    uint_least8_t temp = buf->buffer[buf->idxOut++];
    buf->idxOut &= BUF_MASK;
    return temp;
  }
  return 0;
}

int16_t showSymbolFromRingBuffer(uint16_t symbolNumber, RingBuffer *buf)
{
  if (symbolNumber < getRingBufferCount(buf))
  {
    return buf->buffer[(buf->idxOut + symbolNumber) & BUF_MASK];
  }
  else
  {
    return -1;
  }
}
