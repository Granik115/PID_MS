/*
 * sw_uart.cpp
 *
 *      Author: mmalenko
 *  (c) 2016 NIC SPbETU
 */

#include "sw_uart.h"

bool SoftUart::send(Data<uint_least8_t> *data)
{
  return false;
}

QueueInterface<uint_least8_t>* SoftUart::getRxQueue()
{
  return NULL;
}

bool SoftUart::send(uint16_t byte)
{
  if (!flag_tx_ready)
  {
    user_tx_buffer = byte;
    update();
  }
  return true;
}

void SoftUart::update()
{
  bits_left_in_tx = tx_num_of_bits;
  internal_tx_buffer = static_cast<uint16_t>((user_tx_buffer << 1) | (0x1 << 9));
  flag_tx_ready = true;
}

uint16_t SoftUart::receive()
{
  return 0;
}

uint16_t SoftUart::receive(Data<uint_least8_t>*data)
{
  return 0;
}

bool SoftUart::isBusy()
{
  return flag_tx_ready;
}

SoftUart::SoftUart(GpioPinInterface *pin)
{
  internal_tx_buffer = 0;
  bits_left_in_tx = 0;
  user_tx_buffer = 0;
  timer_tx_ctr = 0;
  this->pin = pin;
  flag_tx_ready = false;
  tx_num_of_bits = 10;
  pin->setHigh();
}

