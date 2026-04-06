/*
 * mkti_sensors_agregator.cpp
 *
 *  Created on: Jan 21, 2025
 *      Author: mvm
 */

#include "encoder_board/mkti_sensors_agregator.h"
#include "digital_inclinometer/ind3.h"
#include "axis_mover/axis_mover_foc.h"

MKTISensorsAgregator::MKTISensorsAgregator(ConnectionInterface &interface, SSIEncoder *azimuthAngleSensor,
                                           SSIEncoder *elevationAngleSensor) :
  EncoderBoard(interface),
  m_inclUpdateTimer(INCLINOMETER_UPDATE_PERIOD),
  m_azimuthAngleSensor(azimuthAngleSensor),
  m_elevationAngleSensor(elevationAngleSensor)
{
  m_dataReliable = true;
  m_azimuthAxisPosition = theodolite_messages_AxisPosition_NORMAL_POSITION;
  m_elevationAxisPosition = theodolite_messages_AxisPosition_NORMAL_POSITION;
  m_inclUpdateTimer.start();
  m_inclinometer = new InD3Inclinometer(getInclinometerConnectionInterface(), IND3_ADDRESS);
}

MKTISensorsAgregator::~MKTISensorsAgregator()
{

}

void MKTISensorsAgregator::updateWithoutRequest()
{
  if (m_inclUpdateTimer.isRunning() && m_inclUpdateTimer.expired())
  {
    m_inclinometer->update();
    m_inclUpdateTimer.restart();
    if (m_inclinometer->isDataReliable())
    {
      m_inclinometerXMills = m_inclinometer->getXMills();
      m_inclinometerYMills = m_inclinometer->getYMills();
      m_inclinometerXADC = m_inclinometerXMills / 1000;
      m_inclinometerYADC = m_inclinometerYMills / 1000;
    }
  }
  if (m_azimuthAngleSensor)
    m_azimuth = FOCAxisMover::codeToMills(m_azimuthAngleSensor->getEncoderCode());
  if (m_elevationAngleSensor)
    m_elevation = FOCAxisMover::codeToMills(m_elevationAngleSensor->getEncoderCode());
  calculateImmediateSpeed();
  if (m_divider == 0)
  {
    m_azimuthMaximumIntervalSpeed = calculateMaximumIntervalSpeed(m_azimuthSpeedQueue, m_azimuthImmediateSpeed);
    m_elevationMaximumIntervalSpeed = calculateMaximumIntervalSpeed(m_elevationSpeedQueue, m_elevationImmediateSpeed);
    m_divider = INTERVAL_SPEED_CALC_DIVIDER;
  }
  --m_divider;
}
