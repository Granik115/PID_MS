///*
// * interfaces.h
// *
// *      Author: mmalenko
// * (c) 2016 NIC SPbETU
// */

#ifndef LOGIC_INTERFACES_INTERFACES_H_
#define LOGIC_INTERFACES_INTERFACES_H_

#include <firmware/queue/queue_interface.h>
#include <firmware/data/data.h>

class ConnectionInterface
{
public:
  virtual ~ConnectionInterface()
  {

  }
  virtual bool send(Data<uint_least8_t> *data)=0;
  virtual bool send(uint_least8_t byte)=0;
  virtual QueueInterface<uint_least8_t>* getRxQueue()=0;
  virtual uint16_t receive()=0;
  virtual uint16_t receive(Data<uint_least8_t> *data)=0;
  virtual bool isBusy()=0;
  virtual bool isPeripheralOk()=0;
};

#endif /* LOGIC_INTERFACES_INTERFACES_H_ */
