/*
 * queue_interface.h
 *
 *  Created on: 12 июл. 2019 г.
 *      Author: mmalenko
 *  (c) 2019 NIC SPbETU    
 */

#ifndef FIRMWARE_QUEUE_QUEUE_INTERFACE_H_
#define FIRMWARE_QUEUE_QUEUE_INTERFACE_H_

#include <stdint.h>

template<typename T>
class QueueInterface
{
public:
  virtual ~QueueInterface()
  {

  }
  virtual void remove(uint16_t number)=0;
  virtual uint16_t getCount()=0;
  virtual void clear()=0;
  virtual bool put(T item)=0;
  virtual T get()=0;
  virtual T show(uint16_t position)=0;
};

#endif /* FIRMWARE_QUEUE_QUEUE_INTERFACE_H_ */
