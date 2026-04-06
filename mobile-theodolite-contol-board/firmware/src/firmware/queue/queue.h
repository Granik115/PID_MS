///*
// * test_queue.h
// *
// *  Created on: 14 июля 2018 г.
// *      Author: mmalenko
//  (c) 2018 NIC SPbETU
// */

#ifndef SRC_THEODOLITE_LOGIC_PC_PROTOCOL_QUEUE_QUEUE_H_
#define SRC_THEODOLITE_LOGIC_PC_PROTOCOL_QUEUE_QUEUE_H_

#include <stdint.h>
#include "queue_interface.h"

template<typename T>
class Queue:
  public QueueInterface<T>
{
  uint16_t size;
  T *buffer;
  uint16_t idxIn;
  uint16_t idxOut;
  public:
  void remove(uint16_t number);
  uint16_t getCount();
  void clear();
  Queue(uint16_t size);
  ~Queue();
  bool put(T item);
  T get();
  T show(uint16_t position);
};

template<typename T>
void Queue<T>::remove(uint16_t number)
{
  idxOut += number;
  idxOut %= size;
}

template<typename T>
uint16_t Queue<T>::getCount()
{
  uint16_t count;
  if (idxIn >= idxOut)
  {
    count = idxIn
    - idxOut;
  }
  else
  {
    count = size - (idxOut - idxIn);
  }
  return count;
}
template<typename T>
void Queue<T>::clear()
{
  idxIn = idxOut = 0;
}
template<typename T>
Queue<T>::Queue(uint16_t size) :
  size(size), buffer(new T[size]), idxIn(0), idxOut(0)
{
  // this->size = size;
  // buffer = new T[size];
  // idxIn = idxOut = 0;
}
template<typename T>
Queue<T>::~Queue()
{
  delete buffer;
}

template<typename T>
bool Queue<T>::put(T item)
{
  if (getCount() < size)
  {
    buffer[idxIn] = item;
    const uint16_t nextIdxIn = (idxIn + 1) % size;
    if (nextIdxIn != idxOut)
    {
      idxIn = nextIdxIn;
    }
    return true;
  }
  return false;
}

template<typename T>
T Queue<T>::get()
{
  T temp;
  if (idxOut != idxIn)
  {
    temp = buffer[idxOut++];
    idxOut %= size;
  }
  return temp;
}

template<typename T>
T Queue<T>::show(uint16_t position)
{
  T temp;
  if (position < getCount())
  {
    const uint16_t index = (idxOut + position) % size;
    temp = buffer[index];
  }
  return temp;
}

#endif /* SRC_THEODOLITE_LOGIC_PC_PROTOCOL_QUEUE_QUEUE_H_ */
