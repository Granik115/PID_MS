///*
// * indication_interface.h
// *
// *  Created on: 13 сент. 2018 г.
// *      Author: mmalenko
//  (c) 2018 NIC SPbETU
// */

#ifndef SRC_THEODOLITE_LOGIC_ENCODER_BOARD_INDICATION_INDICATION_INTERFACE_H_
#define SRC_THEODOLITE_LOGIC_ENCODER_BOARD_INDICATION_INDICATION_INTERFACE_H_

#include "stdint.h"

class IndicationInterface
{
public:
  virtual void setValue(int16_t xvalue, int16_t yvalue)=0;
  virtual void setError()=0;
  virtual void update()=0;
};

IndicationInterface &getIndication();

#endif /* SRC_THEODOLITE_LOGIC_ENCODER_BOARD_INDICATION_INDICATION_INTERFACE_H_ */
