/*
 * pwm.cpp
 *
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */
#include "stddef.h"
#include <theodolite/logic/local_clock/local_clock.h>
#include <theodolite/logic/encoder_board/indication/indication_interface.h>
#include <theodolite/logic/encoder_board_main/encoder_board_protocol/encoder_protocol_interface.h>

void periodicIsrCallEncoderBoard(void)
{
  localClockTimerISR();
  getIndication().update();
  getProtocolInterface().sendStateWithoutRequest();
}
