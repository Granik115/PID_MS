///*(c) 2018 NIC SPbETU
// * pid_saver.cpp
// *
// *  Created on: 25 сент. 2018 г.
// *      Author: mmalenko
// */

#include <firmware/third-party/nanopb/pb_encode.h>
#include <firmware/third-party/nanopb/pb_decode.h>
#include "firmware/queue/queue.h"
#include <theodolite/logic/memory/state_saver.h>

StateSaver::StateSaver(MemoryInterface &eeprom) :
  m_eeprom(eeprom)
{

}

theodolite_messages_State StateSaver::getState()
{
  theodolite_messages_State message = theodolite_messages_State_init_default;
  uint_least8_t protobufBuffer[theodolite_messages_State_size + MAX_VARINT_LENGTH + 3];
  Data<uint_least8_t> data = Data<uint_least8_t>(protobufBuffer, sizeof(protobufBuffer));
  m_eeprom.read(&data, 0);
  Queue<uint_least8_t> queue = Queue<uint_least8_t>(sizeof(protobufBuffer));
  for (uint32_t i = 0; i < sizeof(protobufBuffer); ++i)
  {
    queue.put(data.m_data[i]);
  }
  const ParserStatus status = getUartLinkData(&queue, &data, true, 0xC0);
  if (status == PARSER_STATUS_SUCCESS)
  {
    pb_istream_t stream = pb_istream_from_buffer(data.m_data,
                                                 data.m_size);
    pb_decode(&stream,
              theodolite_messages_State_fields,
              &message);
  }
  return message;
}

bool StateSaver::storeState(theodolite_messages_State& state)
{
  uint_least8_t protobufBuffer[theodolite_messages_State_size];
  pb_ostream_t stream = pb_ostream_from_buffer(protobufBuffer, sizeof(protobufBuffer));
  if (pb_encode(&stream, theodolite_messages_State_fields, &state))
  {
    Data<uint_least8_t> data = Data<uint_least8_t>(protobufBuffer, stream.bytes_written);
    uint_least8_t linkBuffer[sizeof(protobufBuffer) + MAX_VARINT_LENGTH + 3];
    uint16_t writtenSize = uartLinkEncode(&data, linkBuffer, true, 0xC0);
    data.m_data = linkBuffer;
    data.m_size = writtenSize;
    m_eeprom.erase(0, 0);
    return m_eeprom.write(data, 0);
  }
  return false;
}

