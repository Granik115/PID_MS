///*
// * theodolite_messages_processor.h
// *
// *  Created on: 17 мая 2018 г.
// *      Author: maxim
//  (c) 2018 NIC SPbETU
// */

#ifndef SRC_THEODOLITE_LOGIC_PC_PROTOCOL_PROTOBUF_MESSAGES_PROCESSOR_H_
#define SRC_THEODOLITE_LOGIC_PC_PROTOCOL_PROTOBUF_MESSAGES_PROCESSOR_H_

#include <firmware/third-party/nanopb/pb.h>
#include <firmware/third-party/nanopb/pb_encode.h>
#include <firmware/third-party/nanopb/pb_decode.h>
#include <firmware/interfaces/interfaces.h>
#include <firmware/link_level/uart_link.h>

template<typename EnvelopeType>
class ProtobufMessagesProcessor
{
  pb_byte_t m_protobufBuffer[MAX_FRAME_SIZE];
  Data<uint_least8_t> m_data;
  ConnectionInterface &m_interface;
  bool m_varintPrefixed;
  uint_least8_t m_marker;
  const pb_field_t *m_envelopeFields;
  public:
  bool sendMessage(EnvelopeType& message);
  bool fillMessageFromUartLink(EnvelopeType& message);
  ProtobufMessagesProcessor(ConnectionInterface &interface, bool varintPrefixed, uint_least8_t marker,
                            const pb_field_t *envelopeFields);
  ProtobufMessagesProcessor(ConnectionInterface &interface);
};

template<typename EnvelopeType>
ProtobufMessagesProcessor<EnvelopeType>::ProtobufMessagesProcessor(ConnectionInterface &interface,
                                                                   bool varintPrefixed,
                                                                   uint_least8_t marker,
                                                                   const pb_field_t envelopeFields[]) :
  m_interface(interface), m_varintPrefixed(varintPrefixed), m_marker(marker), m_envelopeFields(envelopeFields),
  m_data(Data<uint_least8_t>(static_cast<uint_least8_t*>(m_protobufBuffer), static_cast<size_t>(BUF_SIZE)))
{
}

template<typename EnvelopeType>
bool ProtobufMessagesProcessor<EnvelopeType>::sendMessage(EnvelopeType& message)
{
  pb_ostream_t stream = pb_ostream_from_buffer(m_protobufBuffer,
                                               sizeof(m_protobufBuffer));

  if (pb_encode(&stream, m_envelopeFields, &message))
  {
    Data<uint_least8_t> data = Data<uint_least8_t>(m_protobufBuffer, stream.bytes_written);
    uint_least8_t linkBuffer[MAX_FRAME_SIZE];
    uint16_t writtenSize = uartLinkEncode(&data, linkBuffer, m_varintPrefixed, m_marker);
    data.m_data = linkBuffer;
    data.m_size = writtenSize;
    return m_interface.send(&data);
  }
  return false;
}

template<typename EnvelopeType>
bool ProtobufMessagesProcessor<EnvelopeType>::fillMessageFromUartLink(EnvelopeType& message)
{
  if (m_interface.getRxQueue()->getCount())
  {
    uint_least8_t dataBuffer[MAX_FRAME_SIZE];
    Data<uint_least8_t> data = Data<uint_least8_t>(dataBuffer, sizeof(dataBuffer));
    const ParserStatus status = getUartLinkData(m_interface.getRxQueue(), &data, m_varintPrefixed, m_marker);
    if (status == PARSER_STATUS_SUCCESS)
    {
      pb_istream_t stream = pb_istream_from_buffer(data.m_data,
                                                   data.m_size);
      bool decodeStatus = pb_decode(&stream,
                                    m_envelopeFields,
                                    &message);
      return decodeStatus;
    }
  }
  return false;
}

#endif /* SRC_THEODOLITE_LOGIC_PC_PROTOCOL_PROTOBUF_MESSAGES_PROCESSOR_H_ */
