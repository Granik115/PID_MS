///*
// * uart_link.h
// *
// *      Author: mmalenko
// *      (c) 2015 NIC SPbETU
// */

#ifndef SOURCE_DRIVER_UART_UART_LINK_H_
#define SOURCE_DRIVER_UART_UART_LINK_H_

#include <firmware/ring_buffer/ring_buffer.h>
#include <firmware/data/data.h>
#include <firmware/queue/fast_queue.h>
#include "stddef.h"

#define MAX_FRAME_SIZE (BUF_MASK-4)
const size_t MAX_VARINT_LENGTH = 2;
const size_t MIN_VARINT_LENGTH = 1;

typedef enum
{
  PARSER_STATUS_SUCCESS,
  PARSER_STATUS_NO_MARKER,
  PARSER_STATUS_NO_ENOUGH_DATA,
  PARSER_STATUS_TOO_LARGE_FRAME,
  PARSER_STATUS_CRC_CHECK_ERROR
} ParserStatus;

struct MessageSize
{
  MessageSize(const size_t messageSize, const size_t lengthPrefixSize) :
    m_messageSize(messageSize), m_lengthPrefixSize(lengthPrefixSize)
  {

  }
  const size_t m_messageSize;
  const size_t m_lengthPrefixSize;
};

MessageSize getMessageSize(QueueInterface<uint_least8_t>*, uint16_t offset);
MessageSize getVarintPrefixedMessageSize(QueueInterface<uint_least8_t>*, uint16_t offset);

ParserStatus getUartLinkData(QueueInterface<uint_least8_t> *queue, Data<uint_least8_t> *data, bool varintPrefixed,
                             uint_least8_t marker);
uint16_t uartLinkEncode(Data<uint_least8_t> *data, uint_least8_t*, bool varintPrefixed, uint_least8_t marker);

#endif /* SOURCE_DRIVER_UART_UART_LINK_H_ */
