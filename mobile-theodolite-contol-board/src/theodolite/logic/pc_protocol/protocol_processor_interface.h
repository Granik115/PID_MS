/*
 * protocol_parser_interface.h
 *
 *      Author: mmalenko
 *  (c) 2016 NIC SPbETU
 */

#ifndef LOGIC_PC_PROTOCOL_PROTOCOL_PARSER_INTERFACE_H_
#define LOGIC_PC_PROTOCOL_PROTOCOL_PARSER_INTERFACE_H_

#include <theodolite/logic/pc_protocol/commands.h>
#include <theodolite/logic/pc_protocol/third-party/theodolite_messages.pb.h>

typedef enum
{
  SERIAL_PROTOCOL,
  MODERN_PROTOCOL
} ProtocolType;

class ProtocolProcessorInterface
{
public:
  virtual ~ProtocolProcessorInterface()
  {
  }
  virtual bool sendState()=0;
  virtual bool parseIncomingMessage()=0;
};

ProtocolProcessorInterface *getProcessorInterface(ProtocolType type, CommandsInterface &commands,
                                                  ConnectionInterface &interface,
                                                  GpioPinInterface *receiveCommandLedPin,
                                                  GpioPinInterface*replyCommandLedPin);

ConnectionInterface &getMainConnectionInterface();

#endif /* LOGIC_PC_PROTOCOL_PROTOCOL_PARSER_INTERFACE_H_ */
