///*(c) 2017 NIC SPbETU
// * pc_uart.cpp
// *
// *  Created on: 18 апр. 2018 г.
// *      Author: mmalenko
// */

#include <theodolite/peripheral/hardware.h>
#include <theodolite/peripheral/common/pc_uart/pc_uart.h>

void rxCpuUsart_isr(void);
void txFifoIsrCpuUsart(void);

PcUart pcUart;

ConnectionInterface &getEncoderConnectionInterface()
{
 return pcUart;
}

ConnectionInterface &getCameraConnectionInterface()
{
 return pcUart;
}

//ConnectionInterface &getMainConnectionInterface()
//{
//  return pcUart;
//}
//Переместить в конструктор, в случае корректной инициализации системы до вызова конструкторов глобальных объектов.
void pcUartConfig(uint32_t baudrate)
{

}

PcUart::PcUart()
{

}

bool PcUart::sendPendingByte()
{
  return false;
}

bool PcUart::send(Data<uint_least8_t> *data)
{
  //for (int16_t i = 0; i < data->m_size; i++)
  //{
  //  txQueue.put(data->m_data[i]);
  //}
  //if (data->m_size)
  {
    //CPU_UART.SCIFFTX.bit.TXFFIENA = 1;
  }
  return true;
}

bool PcUart::send(uint_least8_t byte)
{
  return false;
}
QueueInterface<uint_least8_t>* PcUart::getRxQueue()
{
  return &rxQueue;
}
QueueInterface<uint_least8_t>* PcUart::getTxQueue()
{
  return &txQueue;
}
uint16_t PcUart::receive()
{
  return 0;
}
uint16_t PcUart::receive(Data<uint_least8_t> *data)
{
  if (rxQueue.getCount())
  {
    uint32_t i;
    for (i = 0; rxQueue.getCount() && (i < data->m_size); i++)
    {
      data->m_data[i] = rxQueue.get();
    }
    return i;
  }
  return 0;
}

bool PcUart::isBusy()
{
  return false;//CPU_UART.SCIFFTX.bit.TXFFIENA == 1;
}

extern "C"
{
  extern void FMSTR_Isr(void);
}

void rxCpuUsart_isr(void)
{
#ifdef FREEMASTER_PROTOCOL
  FMSTR_Isr();
#else
  //while (CPU_UART.SCIFFRX.bit.RXFIFST)
  {
    //volatile uint16_t byte = CPU_UART.SCIRXBUF.all & 0xFF;
    //pcUart.getRxQueue()->put(byte);
  }
#endif
}

void txFifoIsrCpuUsart(void)
{
 // CPU_UART.SCIFFTX.bit.TXINTCLR = 1;  // Clear SCI Interrupt flag
#ifdef FREEMASTER_PROTOCOL
  FMSTR_Isr();
#else
  while (pcUart.getTxQueue()->getCount())
  {
    //uint16_t byte = pcUart.getTxQueue()->get();
   // CPU_UART.SCITXBUF = byte;
  }
  if (!pcUart.getTxQueue()->getCount())
  {
    //CPU_UART.SCIFFTX.bit.TXFFIENA = 0;
  }
#endif
  //PieCtrlRegs.PIEACK.all |= 0x100;      // Issue PIE ACK
}

