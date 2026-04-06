/*
 * protocol_parser.h
 *
 *      Author: mmalenko
 *  (c) 2016 NIC SPbETU
 */

#ifndef LOGIC_PC_PROTOCOL_PROTOCOL_PARSER_H_
#define LOGIC_PC_PROTOCOL_PROTOCOL_PARSER_H_

#include <firmware/interfaces/interfaces.h>
#include <theodolite/logic/pc_protocol/protocol_processor_interface.h>
#include "firmware/protobuf_messages_processor/protobuf_messages_processor.h"

class ProtocolProcessor:
  public ProtocolProcessorInterface
{
public:
  bool sendState();
  ProtocolProcessor(CommandsInterface &commands, ConnectionInterface &interface, GpioPinInterface *receiveCommandLedPin,
                    GpioPinInterface *replyCommandLedPin);
  bool parseIncomingMessage();
  private:
  ProtobufMessagesProcessor<theodolite_messages_CommandEnvelope> *m_messageProcessor;
  CommandsInterface &m_commands;
  GpioPinInterface *m_receiveCommandLedPin;
  GpioPinInterface*m_replyCommandLedPin;
  bool sendAck(uint32_t);
  bool sendNak(uint32_t);
  CameraFreq getCameraFreqEnum(timesync_messages_Frequency);
  timesync_messages_Frequency getCameraFreqEnum(CameraFreq);
  CameraSyncType getCameraSyncTypeEnum(theodolite_messages_SynchronizationType);
  theodolite_messages_SynchronizationType getCameraSyncTypeEnum(CameraSyncType);
  CameraExpositionType getCameraExpositionTypeEnum(theodolite_messages_ExposureType);
  theodolite_messages_ExposureType getCameraExpositionTypeEnum(CameraExpositionType);
  theodolite_messages_ManualControllerEvent getControllerEventEnum(ControllersEvent);
  theodolite_messages_Test getTestEnum(TestType test);
  bool sendState(uint32_t requestId);
  uint32_t considerAnglePeriod(uint32_t milliseconds, uint32_t delta);
  uint32_t considerAnglePeriod(uint32_t milliseconds);
  static const uint32_t MILLISEC_360_DEGREES = 1296000000;
};

#endif /* LOGIC_PC_PROTOCOL_PROTOCOL_PARCER_H_ */
