/*
 * encoder_board_interface.h
 *
 *  Created on: 13 ���. 2016 �.
 *      Author: mmalenko
 *(c) 2016 NIC SPbETU
 */

#ifndef LOGIC_ENCODER_BOARD_ENCODER_BOARD_INTERFACE_H_
#define LOGIC_ENCODER_BOARD_ENCODER_BOARD_INTERFACE_H_

#include "stdint.h"
#include <theodolite/logic/pc_protocol/third-party/theodolite_messages.pb.h>

class EncoderBoardInterface
{
public:
  virtual ~EncoderBoardInterface()
  {

  }
  virtual int32_t getAzimuth()=0;
  virtual int32_t getElevation()=0;
  virtual theodolite_messages_AxisPosition getAzimuthPosition()=0;
  virtual theodolite_messages_AxisPosition getElevationPosition()=0;
  virtual bool isDataReliable()=0;
  virtual int32_t getInclinometerXMills()=0;
  virtual int32_t getInclinometerYMills()=0;
  virtual uint16_t getInclinometerXADC()=0;
  virtual uint16_t getInclinometerYADC()=0;
  virtual void setUpdateRequest()=0;
  virtual void updateOnRequest()=0;
  virtual void updateWithoutRequest()=0;
  virtual bool isAzimuthAngleOverloaded()=0;
  virtual bool isElevationAngleOverloaded()=0;
  virtual uint32_t getAzimuthMaximumIntervalSpeed()=0;
  virtual uint32_t getElevationMaximumIntervalSpeed()=0;
  virtual int32_t getAzimuthImmediateSpeed()=0;
  virtual int32_t getElevationImmediateSpeed()=0;
};
#endif /* LOGIC_ENCODER_BOARD_ENCODER_BOARD_INTERFACE_H_ */
