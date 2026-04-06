/* $Id: cpuprotocol.c dpshenichnov  */
/*!
 * \file cpuprotocol.c
 * \brief
 *
 * PROJ: BFT
 *
 * (c) 2015 NIC SPbETU
 * ------------------------------------------------------------------------ */

#include "cpuprotocol.h"
#include "time.h"
#include "firmware/ring_buffer/ring_buffer.h"
#include "hardware.h"
#include "time_module/synchronization/sync.h"
#include "time_module/errors/errors.h"
#include "time_module/time/iso_time.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "stdio.h"

namespace
{
  void setMode(const timesync_messages_SetMode&);
  void setTime(const timesync_messages_SetTime&);
  void replyDebugInfo(void);
  void parseMessage(const timesync_messages_Envelope&);
  bool fillMessageFromUartLink(timesync_messages_Envelope &message);
  SevStatus convertSevStatusType(timesync_messages_TimeSourceType status);
  timesync_messages_TimeSourceType convertSevStatusType(SevStatus status);

  void setMode(const timesync_messages_SetMode &mode)
  {
    syncSetFrequency(mode.frequency);
    //setFrequencyIndication(mode.frequency);
    setSyncSource(convertSevStatusType(mode.time_source_type));
    if (mode.has_theodolite_number)
    {
      setShiftValue(mode.theodolite_number, mode.impulse_shift_time_us);
    }
    else
    {
      const uint32_t MAX_THEODOLITE_NUMBER = 4;
      for (uint8_t i = 1; i < MAX_THEODOLITE_NUMBER + 1; i++)
      {
        setShiftValue(i, mode.impulse_shift_time_us);
      }
    }
    //setDigitalSevInterface(mode.digital_sev_interface_type);
    setShiftMode(mode.synchronization_shift_mode);
  }

  void setTime(const timesync_messages_SetTime &time)
  {
    PulseTime pulseTime = {};
    if (decodeIsoTime(&pulseTime, time.time))
    {
      pulseTime.time.tm_sec = 0;
      setExplicitTime(mktime(&pulseTime.time));
    }
  }

  bool fillMessageFromUartLink(timesync_messages_Envelope &message)
  {
    /*if (getRingBufferCount(getEthernetRxBuffer()))
    {
      uint8_t dataBuffer[MAX_DATA_SIZE];
      Data data =
      {dataBuffer, MAX_DATA_SIZE};
      const uint8_t receiveDataSize = getUartLinkData(getEthernetRxBuffer(), &data);
      if (receiveDataSize)
      {
        pb_istream_t stream = pb_istream_from_buffer(data.data, receiveDataSize);
        return pb_decode(&stream, timesync_messages_Envelope_fields, &message);;
      }
    }*/
    return false;
  }

  SevStatus convertSevStatusType(timesync_messages_TimeSourceType status)
  {
    return static_cast<SevStatus>(status);
  }
  timesync_messages_TimeSourceType convertSevStatusType(SevStatus status)
  {
    return static_cast<timesync_messages_TimeSourceType>(status);
  }

  void replyDebugInfo()
  {
    timesync_messages_Envelope debugInfo = {};
    debugInfo.has_debug_info = true;
    debugInfo.debug_info.has_navigation_data = true;
    debugInfo.debug_info.navigation_data.correctness = getGnssRmcStruct()->warn == RMC_RELIABLE_FLAG;
    debugInfo.debug_info.navigation_data.date = getGnssRmcStruct()->date;
    debugInfo.debug_info.navigation_data.time = getGnssRmcStruct()->time;
    debugInfo.debug_info.navigation_data.latitude = getGnssRmcStruct()->latitude;
    debugInfo.debug_info.navigation_data.longitude = getGnssRmcStruct()->longitude;
    const nmeap_gga_t *gga = getGnssGgaStruct();
    if (gga)
    {
      snprintf(debugInfo.debug_info.gga, sizeof(debugInfo.debug_info.gga), "sat:%d, quality:%d\n", gga->satellites, gga->quality);
      debugInfo.debug_info.has_gga = true;
    }
    uint8_t dataBuffer[MAX_DATA_SIZE];
    pb_ostream_t stream = pb_ostream_from_buffer(dataBuffer,
                                                 sizeof(dataBuffer));
    if (pb_encode(&stream, timesync_messages_Envelope_fields, &debugInfo))
    {
      sendFrame(dataBuffer, stream.bytes_written, UART_CHANNEL1);
    }
  }

  void parseMessage(const timesync_messages_Envelope &message)
  {
    if (message.has_set_mode)
    {
      setMode(message.set_mode);
    }
    if (message.has_set_time)
    {
      setTime(message.set_time);
    }
    if (message.has_get_debug_info)
    {
      replyDebugInfo();
    }
  }
}

void protocolTask()
{
  timesync_messages_Envelope message = {};
  if (fillMessageFromUartLink(message))
  {
    parseMessage(message);
  }
}

uint32_t getChannelNumber(TimeInterface interface)
{
  uint32_t channel = 0;
  switch (interface)
  {
    case UART_CHANNEL1:
      channel = 1;
      break;
    case UART_CHANNEL2:
      channel = 2;
      break;
    case UART_CHANNEL3:
      channel = 3;
      break;
    case UART_CHANNEL4:
      channel = 4;
      break;
    default:
      break;
  }
  return channel;
}

void sendTime(const PulseTime &time, TimeInterface interface)
{
  timesync_messages_Envelope envelope = {};
  if (!encodeIsoTime(time, envelope.synchroimpulse.time))
  {
    return;
  }
  envelope.has_synchroimpulse = true;
  static uint32_t synchroCounter = 0;
  envelope.synchroimpulse.synchroimpulse_number = synchroCounter;
  ++synchroCounter;
  timesync_messages_State &state = envelope.synchroimpulse.state;
  state.frequency = getSyncPulseFrequency();
  state.available_time_sources_count = 0;
  if (isAnalogSevAlailable())
  {
    state.available_time_sources[state.available_time_sources_count++] =
    timesync_messages_TimeSourceType_ANALOG_SEV;
  }
  if (isDigitalSevAlailable())
  {
    state.available_time_sources[state.available_time_sources_count++] =
    timesync_messages_TimeSourceType_DIGITAL_SEV;
  }
  if (isGnssAlailable())
  {
    state.available_time_sources[state.available_time_sources_count++] =
    timesync_messages_TimeSourceType_GNSS;
  }
  state.time_source_type = convertSevStatusType(getSevStatus());
  envelope.synchroimpulse.state.synchronization_shift_mode = isShiftMode();
  if (interface == UART_CHANNEL1)
  {
    const uint32_t errorsFieldSize = sizeof(state.errors) / sizeof(timesync_messages_ErrorType);
    for (uint8_t i = 0; (i < errorsFieldSize) && (getErrorCount()); i++)
    {
      state.errors[i] = getError();
      state.errors_count = i + 1;
    }
  }
  state.digital_sev_interface_type = getDigitalSevInterface();
  state.impulse_shift_time_us = getShiftValue(getChannelNumber(interface));
  uint8_t dataBuffer[MAX_DATA_SIZE];
  pb_ostream_t stream = pb_ostream_from_buffer(dataBuffer,
                                               sizeof(dataBuffer));
  if (pb_encode(&stream, timesync_messages_Envelope_fields, &envelope))
  {
    if (sendFrame(dataBuffer, stream.bytes_written, interface) == false)
    {
      sendError(timesync_messages_ErrorType_UNKNOWN_ERROR);
    }
  }
}

