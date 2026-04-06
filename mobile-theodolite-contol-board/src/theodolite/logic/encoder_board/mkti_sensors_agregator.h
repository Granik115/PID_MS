/*
 * mkti_sensors_agregator.h
 *
 *  Created on: Jan 21, 2025
 *      Author: mvm
 */

#ifndef THEODOLITE_LOGIC_ENCODER_BOARD_MKTI_SENSORS_AGREGATOR_H_
#define THEODOLITE_LOGIC_ENCODER_BOARD_MKTI_SENSORS_AGREGATOR_H_

#include <theodolite/logic/encoder_board/encoder_board.h>
#include "foc/ssi_encoder/ssi_encoder.h"
#include <theodolite/logic/encoder_board/inclinometer/inclinometer_interface.h>

class MKTISensorsAgregator:
  public EncoderBoard
{
  static const uint32_t INCLINOMETER_UPDATE_PERIOD = 100;
  Timer m_inclUpdateTimer;
  SSIEncoder *m_azimuthAngleSensor;
  SSIEncoder *m_elevationAngleSensor;
  InclinometerInterface *m_inclinometer;
  public:
  ~MKTISensorsAgregator();
  MKTISensorsAgregator(ConnectionInterface &interface, SSIEncoder *azimuthAngleSensor,
                       SSIEncoder *elevationAngleSensor);
  void updateWithoutRequest();
};

#endif /* THEODOLITE_LOGIC_ENCODER_BOARD_MKTI_SENSORS_AGREGATOR_H_ */
