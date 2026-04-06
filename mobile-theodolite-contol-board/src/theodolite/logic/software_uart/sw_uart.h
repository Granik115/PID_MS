/*
 * sw_uart.h
 *
 *      Author: mmalenko
 * (c) 2016 NIC SPbETU
 */

#ifndef PERIPHERAL_SOFTWARE_UART_SW_UART_H_
#define PERIPHERAL_SOFTWARE_UART_SW_UART_H_

#include <theodolite/logic/periph_bridge/gpio/gpio.h>
#include <firmware/interfaces/interfaces.h>
#include <firmware/queue/fast_queue.h>

class SoftUart:
  public ConnectionInterface
{
  GpioPinInterface *pin;
  bool flag_tx_ready;
  uint16_t timer_tx_ctr;
  uint16_t bits_left_in_tx;
  uint16_t tx_num_of_bits;
  uint16_t internal_tx_buffer;
  uint16_t user_tx_buffer;
  void update();
  public:
  bool send(Data<uint_least8_t> *data);
  QueueInterface<uint_least8_t>* getRxQueue();
  uint16_t receive();
  uint16_t receive(Data<uint_least8_t>*data);
  explicit SoftUart(GpioPinInterface *pin);
  void isrCallFunction()
  {
    if (flag_tx_ready)
    {
      uint16_t mask = internal_tx_buffer & 1;
      internal_tx_buffer = static_cast<uint16_t>(internal_tx_buffer >> 1);
      if (mask)
      {
        pin->setHigh();
      }
      else
      {
        pin->setLow();
      }
      if (--bits_left_in_tx <= 0)
      {
        flag_tx_ready = false;
      }
    }
  }
  bool send(uint16_t byte);
  bool isBusy();
};

void setSoftUart(SoftUart *serial);

#endif /* PERIPHERAL_SOFTWARE_UART_SW_UART_H_ */
