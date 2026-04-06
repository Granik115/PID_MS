///*(c) 2018 NIC SPbETU
// * serial_protocol.h
// *
// *  Created on: 15 авг. 2018 г.
// *      Author: maxim
// */

#ifndef SRC_THEODOLITE_LOGIC_PC_PROTOCOL_SERIAL_PROTOCOL_SERIAL_PROTOCOL_H_
#define SRC_THEODOLITE_LOGIC_PC_PROTOCOL_SERIAL_PROTOCOL_SERIAL_PROTOCOL_H_

#include <theodolite/logic/pc_protocol/protocol_processor_interface.h>
#include <firmware/interfaces/interfaces.h>

class SerialProtocolProcessor:
  public ProtocolProcessorInterface
{
  ConnectionInterface &m_interface;
  CommandsInterface &commands;
  GpioPinInterface *receiveCommandLedPin;
  GpioPinInterface*replyCommandLedPin;
  Timer *protectTimer;
  uint_least8_t crcMod2(uint_least8_t *buff, uint16_t len);
  uint_least8_t crcParity(QueueInterface<uint_least8_t> *, uint16_t len);
  bool verifyRequestZero();
  void cleanBuffer();
  bool sendAck(uint_least8_t ackMarker);
  bool sendBlockingMode();
  public:
  bool sendState();
  SerialProtocolProcessor(CommandsInterface &commands, ConnectionInterface &interface,
                          GpioPinInterface *receiveCommandLedPin,
                          GpioPinInterface *replyCommandLedPin);
  bool parseIncomingMessage();

};

#endif /* SRC_THEODOLITE_LOGIC_PC_PROTOCOL_SERIAL_PROTOCOL_SERIAL_PROTOCOL_H_ */
