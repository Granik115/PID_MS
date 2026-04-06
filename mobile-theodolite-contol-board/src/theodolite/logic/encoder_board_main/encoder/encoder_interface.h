///*
// * encoder_hardware_interface.h
// *
// *  Created on: 13 сент. 2018 г.
// *      Author: mmalenko
//(c) 2018 NIC SPbETU
// */

#ifndef SRC_THEODOLITE_PERIPHERAL_ENCODER_BOARD_ENCODER_ENCODER_HARDWARE_INTERFACE_H_
#define SRC_THEODOLITE_PERIPHERAL_ENCODER_BOARD_ENCODER_ENCODER_HARDWARE_INTERFACE_H_

#include <stdint.h>
#include <theodolite/logic/pc_protocol/third-party/theodolite_messages.pb.h>

class EncoderInterface
{
public:
  virtual uint32_t getCount() =0;
  virtual void calculatePosition(uint32_t currentTimerCount)=0;
  virtual void init()=0;
  virtual int32_t getVelocity()=0;
  virtual theodolite_messages_AxisPosition getAxisPosition()=0;
  virtual bool isZero()=0;
};

void encodersConfig();

#endif /* SRC_THEODOLITE_PERIPHERAL_ENCODER_BOARD_ENCODER_ENCODER_HARDWARE_INTERFACE_H_ */
