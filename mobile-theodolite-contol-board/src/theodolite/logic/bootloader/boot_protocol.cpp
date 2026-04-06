///*
// * boot_protocol.cpp
// *
// *  Created on: 21 дек. 2018 г.
// *      Author: mmalenko
// * (c) 2018 NIC SPbETU
// */

#include <theodolite/logic/bootloader/bootloader_protocol.h>

bool BootloaderProtocol::parseIncomingMessage()
{
  CommandEnvelope message =
  {0};
  if (m_encoderMessagesProcessor != NULL && m_encoderMessagesProcessor->fillMessageFromUartLink(message))
  {
    m_pcMessagesProcessor.sendMessage(message);
  }
  if (m_pcMessagesProcessor.fillMessageFromUartLink(message) == true)
  {
    bool status = false;
    if (message.target.target_board == m_boardType)
    {
      CommandEnvelope replyEnvelope =
      {0};
      replyEnvelope.request_id = message.request_id;
      replyEnvelope.target.target_board = message.target.target_board;
      replyEnvelope.target.sector = message.target.sector;
      if (message.has_state_req)
      {
        return sendState(replyEnvelope);
      }
      else if (message.has_segment)
      {
        m_commands.processFirmwareSegment(message.target.sector, message.segment);
        return sendState(replyEnvelope);
      }
      else if (message.has_erase_req)
      {
        m_commands.erase(message.target.sector);
        return sendState(replyEnvelope);
      }
      else if (message.has_verify_crc_req)
      {
        status = m_commands.verifyCrc(message.target.sector, message.verify_crc_req.crc);
        return replyVerifyCrc(replyEnvelope, message.target.sector, status);
      }
      else if (message.has_start_application_req)
      {
        m_commands.startApplication();
      }
      else if (message.has_version_req)
      {
        replyBootloaderVersion(replyEnvelope);
      }
    }
    else if (message.target.target_board == TargetBoard_ENCODER_BOARD && m_encoderMessagesProcessor != NULL)
    {
      m_commands.setInIdle();
      return m_encoderMessagesProcessor->sendMessage(message);
    }
  }
  return false;
}

bool BootloaderProtocol::sendMessageToEncoderBoard(CommandEnvelope &message)
{
  return false;
}

BootloaderProtocol::BootloaderProtocol(TargetBoard board, BootloaderCommandsInterface &commands,
                                       ProtobufMessagesProcessor<CommandEnvelope> &pcMessagesProcessor,
                                       ProtobufMessagesProcessor<CommandEnvelope> *encoderBoardMessagesProcessor) :
  m_commands(commands), m_pcMessagesProcessor(pcMessagesProcessor),
  m_encoderMessagesProcessor(encoderBoardMessagesProcessor), m_boardType(board)
{

}

bool BootloaderProtocol::replyVerifyCrc(CommandEnvelope &envelope, SectorType sector, bool isCrcOk)
{
  envelope.has_verify_status = true;
  envelope.verify_status.is_crc_ok = isCrcOk;
  return m_pcMessagesProcessor.sendMessage(envelope);
}

bool BootloaderProtocol::sendState(CommandEnvelope &envelope)
{
  envelope.has_state = true;
  envelope.state = m_commands.getState(envelope.target.sector);
  return m_pcMessagesProcessor.sendMessage(envelope);
}

bool BootloaderProtocol::sendState()
{
  return false;
}

bool BootloaderProtocol::replyBootloaderVersion(CommandEnvelope &envelope)
{
  Data<char> appVersionData = Data<char>(envelope.version_info.app_version,
                                         sizeof(envelope.version_info.app_version));
  envelope.has_version_info = true;
  const char* boot_string = m_commands.getBootloaderVersionString();
  for (uint16_t i = 0; boot_string[i] && i < sizeof(envelope.version_info.boot_version); i++)
  {
    envelope.version_info.boot_version[i] = boot_string[i];
  }
  m_commands.getApplicationVersionString(appVersionData);
  envelope.version_info.platform = m_commands.getBootloaderPlatformType();
  envelope.version_info.app_version[sizeof(envelope.version_info.app_version) - 1] = 0;
  envelope.version_info.boot_version[sizeof(envelope.version_info.boot_version) - 1] = 0;
  return m_pcMessagesProcessor.sendMessage(envelope);
}
