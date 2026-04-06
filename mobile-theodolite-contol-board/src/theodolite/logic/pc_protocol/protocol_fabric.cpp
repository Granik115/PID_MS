///*(c) 2018 NIC SPbETU
// * protocol_fabric.cpp
// *
// *  Created on: 15 авг. 2018 г.
// *      Author: maxim
// */

#include <theodolite/logic/pc_protocol/protocol_processor.h>
#include <theodolite/logic/pc_protocol/serial_protocol/serial_protocol.h>

ProtocolProcessorInterface *getProcessorInterface(ProtocolType type, CommandsInterface &commands,
                                                  ConnectionInterface &interface,
                                                  GpioPinInterface *receiveCommandLedPin,
                                                  GpioPinInterface*replyCommandLedPin)
{
  ProtocolProcessorInterface *protocol = NULL;
  if (type == SERIAL_PROTOCOL)
  {
    protocol = new SerialProtocolProcessor(commands, interface, receiveCommandLedPin, replyCommandLedPin);
  }
  else if (type == MODERN_PROTOCOL)
  {
    protocol = new ProtocolProcessor(commands, interface, receiveCommandLedPin, replyCommandLedPin);
  }
  return protocol;
}
