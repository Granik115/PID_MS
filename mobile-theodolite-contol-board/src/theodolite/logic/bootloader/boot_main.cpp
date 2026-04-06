///*
// * boot_main.cpp
// *
// *  Created on: 21 дек. 2018 г.
// *      Author: mmalenko
// * (c) 2018 NIC SPbETU
// */

#include <theodolite/peripheral/hardware.h>
#include "theodolite/logic/bootloader/boot_hardware.h"
#include <theodolite/logic/bootloader/third-party/boot.pb.h>
#include <theodolite/logic/periph_bridge/gpio/gpio.h>
#include <firmware/interfaces/interfaces.h>
#include <theodolite/logic/periph_bridge/uart.h>
#include <firmware/protobuf_messages_processor/protobuf_messages_processor.h>
#include <theodolite/logic/bootloader/bootloader_protocol.h>
#include <theodolite/peripheral/bootloader_common/boot_gpio.h>
#include "theodolite/logic/bootloader/platform_interface.h"
#include "theodolite/logic/bootloader/bootloader_commands.h"
#include "theodolite/logic/sys_config/sys_cfg.h"
#include "theodolite/logic/local_clock/local_clock.h"

const int PERIODIC_SEND_TIME_MS = 50;
static const uint_least8_t BOOTLOADER_MARKER = 0xB0;
extern void updatePeripheralTask();

volatile bool isOk;

int main(void)
{
  bootHardwareConfiguration();
  BootloaderCommands commands = BootloaderCommands(getPlatform());
  ConnectionInterface &pcConnection = getMainConnectionInterface();
  ProtobufMessagesProcessor<CommandEnvelope> pcMessagesProcessor = ProtobufMessagesProcessor<CommandEnvelope>(
  pcConnection,
  true,
  BOOTLOADER_MARKER,
  CommandEnvelope_fields);

#ifndef ENCODER_BOARD
  ConnectionInterface &encoderBoardConnection = getEncoderConnectionInterface();
  ProtobufMessagesProcessor<CommandEnvelope> encoderMessagesProcessor = ProtobufMessagesProcessor<CommandEnvelope>(
  encoderBoardConnection,
  true,
  BOOTLOADER_MARKER,
  CommandEnvelope_fields);
  ProtobufMessagesProcessor<CommandEnvelope> *p_encoderMessagesProcessor = &encoderMessagesProcessor;
  const TargetBoard board = TargetBoard_PROCESSOR_BOARD;

#else
  const TargetBoard board = TargetBoard_ENCODER_BOARD;
  ProtobufMessagesProcessor<CommandEnvelope> *p_encoderMessagesProcessor = NULL;
#endif

  BootloaderProtocol protocol = BootloaderProtocol(board, commands,
                                                   pcMessagesProcessor,
                                                   p_encoderMessagesProcessor);

  Timer blinkTimer = Timer(PERIODIC_SEND_TIME_MS);
  GpioPinInterface *pin = getPin(SEND_REPLY_LED_PIN);
  while (1)
  {
    if (blinkTimer.expired())
    {
      blinkTimer.reset();
      pin->setLevel(!pin->getLevel());
    }
    protocol.parseIncomingMessage();
    commands.update();
    updatePeripheralTask();
  }
}

