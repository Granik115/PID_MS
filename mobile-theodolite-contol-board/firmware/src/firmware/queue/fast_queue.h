///*
// * test_queue.h
// *
// *  Created on: 14 июля 2018 г.
// *      Author: mmalenko
//  (c) 2018 NIC SPbETU
// */

#ifndef SRC_THEODOLITE_LOGIC_PC_PROTOCOL_FAST_QUEUE_QUEUE_H_
#define SRC_THEODOLITE_LOGIC_PC_PROTOCOL_FAST_QUEUE_QUEUE_H_

#include <stdint.h>
#include "queue_interface.h"

template<typename T, uint32_t SIZE_MASK>
class FastQueue:
  public QueueInterface<T>
{
  T buffer[SIZE_MASK + 1];
  uint16_t idxIn;
  uint16_t idxOut;
  public:
  void remove(uint16_t number);
  uint16_t getCount();
  void clear();
  FastQueue();
  virtual ~FastQueue();
  bool put(T item);
  T get();
  T show(uint16_t position);
};

template<typename T, uint32_t SIZE_MASK>
void FastQueue<T, SIZE_MASK>::remove(uint16_t number)
{
  idxOut += number;
  idxOut &= SIZE_MASK;
}

template<typename T, uint32_t SIZE_MASK>
uint16_t FastQueue<T, SIZE_MASK>::getCount()
{
  return (idxIn - idxOut) & SIZE_MASK;
}

template<typename T, uint32_t SIZE_MASK>
void FastQueue<T, SIZE_MASK>::clear()
{
  idxIn = idxOut = 0;
}

template<typename T, uint32_t SIZE_MASK>
FastQueue<T, SIZE_MASK>::FastQueue() :
  idxIn(0), idxOut(0)
{
}

template<typename T, uint32_t SIZE_MASK>
FastQueue<T, SIZE_MASK>::~FastQueue()
{
}

template<typename T, uint32_t SIZE_MASK>
bool FastQueue<T, SIZE_MASK>::put(T item)
{
  buffer[idxIn++] = item;
  idxIn &= SIZE_MASK;
  return true;
}

template<typename T, uint32_t SIZE_MASK>
T FastQueue<T, SIZE_MASK>::get()
{
  T temp = buffer[idxOut++];
  idxOut &= SIZE_MASK;
  return temp;
}

template<typename T, uint32_t SIZE_MASK>
T FastQueue<T, SIZE_MASK>::show(uint16_t position)
{
  const uint16_t index = (idxOut + position) & SIZE_MASK;
  return buffer[index];
}

#endif /* SRC_THEODOLITE_LOGIC_PC_PROTOCOL_FAST_QUEUE_QUEUE_H_ */
