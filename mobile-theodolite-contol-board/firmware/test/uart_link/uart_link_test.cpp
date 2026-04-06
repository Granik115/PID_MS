///*
// * uart_link_test.cpp
// *
// *  Created on: 4 дек. 2018 г.
// *      Author: mmalenko
// *  (c) 2018 NIC SPbETU
// */

#include <gtest/gtest.h>
#include <firmware/queue/fast_queue.h>
#include <firmware/link_level/uart_link.h>
#include <firmware/third-party/nanopb/pb_encode.h>

TEST(UartLinkTest, GetMessageSize)
{
  FastQueue<uint_least8_t,0x7F> queue;
  pb_byte_t varintBuffer[10];
  pb_ostream_t os = pb_ostream_from_buffer(varintBuffer,
                                           sizeof(varintBuffer));
  uint64_t messageSize = 6;

  ASSERT_EQ(pb_encode_varint(&os, messageSize), true);

  for (uint16_t i = 0; i < os.bytes_written; i++)
  {
    queue.put(varintBuffer[i]);
  }

  MessageSize msgSize = getVarintPrefixedMessageSize(&queue, 0);

  ASSERT_EQ(msgSize.m_messageSize, messageSize);
  ASSERT_EQ(msgSize.m_lengthPrefixSize, 1);

  {
    pb_ostream_t os = pb_ostream_from_buffer(varintBuffer,
                                             sizeof(varintBuffer));
    messageSize = 300;
    queue.clear();
    ASSERT_EQ(pb_encode_varint(&os, messageSize), true);
    for (uint16_t i = 0; i < os.bytes_written; i++)
    {
      queue.put(varintBuffer[i]);
    }
    MessageSize msgSize = getVarintPrefixedMessageSize(&queue, 0);
    ASSERT_EQ(msgSize.m_messageSize, messageSize);
    ASSERT_EQ(msgSize.m_lengthPrefixSize, 2);
  }

}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
