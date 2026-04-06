/*
 * inclinometer_interface.h
 *
 *  Created on: 17 мая 2018 г.
 *      Author: mmalenko
 *  (c) 2018 NIC SPbETU    
 */

#ifndef SRC_THEODOLITE_PERIPHERAL_ENCODER_BOARD_INCLINOMETER_INCLINOMETER_INTERFACE_H_
#define SRC_THEODOLITE_PERIPHERAL_ENCODER_BOARD_INCLINOMETER_INCLINOMETER_INTERFACE_H_

#include "stdint.h"

class InclinometerInterface
{
public:
  virtual ~InclinometerInterface()
  {

  }
  virtual void update()=0;
  virtual int32_t getXMills()=0;
  virtual int32_t getYMills()=0;
  virtual bool isDataReliable()=0;
};

void inclinometerConfig();
InclinometerInterface & getDigitalInclinometer();
InclinometerInterface & getAnalogInclinometer();

#endif /* SRC_THEODOLITE_PERIPHERAL_ENCODER_BOARD_INCLINOMETER_INCLINOMETER_INTERFACE_H_ */
