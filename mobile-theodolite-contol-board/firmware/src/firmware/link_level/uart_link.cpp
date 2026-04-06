///*
// * uart_link.c
// *
// *  Created on: 20 ���. 2015 �.
// *      Author: mmalenko
// (c) 2015 NIC SPbETU
// */

#include "uart_link.h"
#include "crc16.h"
#include <firmware/queue/fast_queue.h>
#include <firmware/third-party/nanopb/pb.h>
#include <firmware/third-party/nanopb/pb_decode.h>
#include <firmware/third-party/nanopb/pb_encode.h>

MessageSize getMessageSize(QueueInterface<uint_least8_t> *queue, uint16_t offset)
{
  const size_t headerSize = offset + 1;
  if (queue->getCount() < headerSize)
  {
    return MessageSize(0, 0);
  }
  return MessageSize(queue->show(offset), 1);
}

MessageSize getVarintPrefixedMessageSize(QueueInterface<uint_least8_t> *queue, uint16_t offset)
{
  const uint16_t count = queue->getCount();
  if (count < offset + MIN_VARINT_LENGTH)
  {
    return MessageSize(0, 0);
  }
  pb_byte_t varintBuffer[MAX_VARINT_LENGTH] =
  {0};
  for (uint16_t i = 0; i < MAX_VARINT_LENGTH && i < count - offset; ++i)
  {
    varintBuffer[i] = queue->show(offset + i);
  }
  pb_istream_t is = pb_istream_from_buffer(varintBuffer,
                                           MAX_VARINT_LENGTH);
  uint64_t messageSize = 0;
  if (!pb_decode_varint(&is, &messageSize))
  {
    return MessageSize(0, 0);
  }
  pb_ostream_t os = pb_ostream_from_buffer(varintBuffer,
                                           MAX_VARINT_LENGTH);
  if (!pb_encode_varint(&os, messageSize))
  {
    return MessageSize(0, 0);
  }
  return MessageSize(static_cast<size_t>(messageSize), os.bytes_written);
}

uint16_t encodeVarint(pb_byte_t *buf, size_t size, uint64_t value)
{
  pb_ostream_t os = pb_ostream_from_buffer(buf, size);
  if (pb_encode_varint(&os, value))
  {
    return os.bytes_written;
  }
  return 0;
}

ParserStatus getUartLinkData(QueueInterface<uint_least8_t> *queue, Data<uint_least8_t> *data, bool varintPrefixed,
                             uint_least8_t marker)
{
  const uint16_t count = queue->getCount();
  if (count > 4)
  {
    if (queue->show(0) == marker)
    {
      const uint16_t SIZE_OFFSET = 1;
      const MessageSize size = varintPrefixed ? getVarintPrefixedMessageSize(queue, SIZE_OFFSET) :
                                                getMessageSize(queue, SIZE_OFFSET);
      const uint16_t PACKET_SIZE = size.m_messageSize + size.m_lengthPrefixSize + 3;
      if ((data->m_size < size.m_messageSize) || ((PACKET_SIZE) > BUF_MASK))
      {
        queue->remove(1);
        return PARSER_STATUS_TOO_LARGE_FRAME;
      }
      if (count >= PACKET_SIZE)
      {
        const uint16_t crc_h = queue->show(PACKET_SIZE - 2);
        const uint16_t crc_l = queue->show(PACKET_SIZE - 1);
        const uint16_t crc16 = static_cast<uint16_t>((crc_h << 8) | crc_l);
        if (crc16 == crc16Calculate(queue, PACKET_SIZE - 2, 0))
        {
          queue->remove(size.m_lengthPrefixSize + 1);
          for (uint_least8_t i = 0; i < size.m_messageSize; ++i)
          {
            *(data->m_data + i) = queue->get();
          }
          queue->remove(2);
          data->m_size = size.m_messageSize;
          return PARSER_STATUS_SUCCESS;
        }
        else
        {
          queue->remove(1);
          return PARSER_STATUS_CRC_CHECK_ERROR;
        }
      }
      return PARSER_STATUS_NO_ENOUGH_DATA;
    }
    else
    {
      queue->remove(1);
      return PARSER_STATUS_NO_MARKER;
    }
  }
  return PARSER_STATUS_NO_ENOUGH_DATA;
}

uint16_t uartLinkEncode(Data<uint_least8_t> *data,
                        uint_least8_t *outBuffer,
                        bool varintPrefixed, uint_least8_t marker)
{
  uint16_t bufferCursor = 0;
  *(outBuffer + bufferCursor++) = marker;
  if (varintPrefixed)
  {
    pb_byte_t varintBuffer[MAX_VARINT_LENGTH] =
    {0};
    const uint16_t varintLenght = encodeVarint(varintBuffer, MAX_VARINT_LENGTH, data->m_size);
    for (uint16_t i = 0; i < varintLenght; ++i)
    {
      *(outBuffer + bufferCursor++) = varintBuffer[i];
    }
  }
  else
  {
    *(outBuffer + bufferCursor++) = static_cast<uint_least8_t>(data->m_size);
  }
  for (uint_least8_t i = 0; i < data->m_size; i++)
  {
    *(outBuffer + bufferCursor++) = *(data->m_data + i);
  }
  const uint16_t crc = crc16(outBuffer, bufferCursor);
  *(outBuffer + bufferCursor++) = static_cast<uint_least8_t>((crc >> 8) & 0xFF);
  *(outBuffer + bufferCursor++) = static_cast<uint_least8_t>(crc & 0xFF);
  return bufferCursor;
}

