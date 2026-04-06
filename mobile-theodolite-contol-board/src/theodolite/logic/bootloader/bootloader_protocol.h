///*
// * BootloaderProtocol.h
// *
// *  Created on: 25 дек. 2018 г.
// *      Author: mmalenko
// *  (c) 2018 NIC SPbETU
// */

#ifndef SRC_THEODOLITE_LOGIC_BOOTLOADER_BOOTLOADER_PROTOCOL_H_
#define SRC_THEODOLITE_LOGIC_BOOTLOADER_BOOTLOADER_PROTOCOL_H_

#include <theodolite/logic/pc_protocol/protocol_processor_interface.h>
#include <theodolite/logic/bootloader/third-party/boot.pb.h>
#include <firmware/protobuf_messages_processor/protobuf_messages_processor.h>
#include "bootloader_commands_interface.h"

class BootloaderProtocol:
  public ProtocolProcessorInterface
{
  ProtobufMessagesProcessor<CommandEnvelope> &m_pcMessagesProcessor;
  ProtobufMessagesProcessor<CommandEnvelope> *m_encoderMessagesProcessor;
  BootloaderCommandsInterface &m_commands;
  TargetBoard m_boardType;
  bool sendMessageToEncoderBoard(CommandEnvelope &message);
  bool replyVerifyCrc(CommandEnvelope &envelope, SectorType sector, bool isCrcOk);
  bool sendState(CommandEnvelope &);
  bool replyBootloaderVersion(CommandEnvelope &envelope);
  public:
  BootloaderProtocol(TargetBoard board, BootloaderCommandsInterface &commands,
                     ProtobufMessagesProcessor<CommandEnvelope> &pcMessagesProcessor,
                     ProtobufMessagesProcessor<CommandEnvelope> *encoderMessagesProcessor);
  bool sendState();
  bool parseIncomingMessage();
};

#endif /* SRC_THEODOLITE_LOGIC_BOOTLOADER_BOOTLOADER_PROTOCOL_H_ */
