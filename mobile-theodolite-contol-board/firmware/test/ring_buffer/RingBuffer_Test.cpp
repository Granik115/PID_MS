/*!
 * \file RingBuffer_Test.cpp
 * \brief Test for RingBuffer
 *
 * PROJ: ОЭС ТИК
 *
 * (c) 2016 NIC SPbETU
 * ------------------------------------------------------------------------ */

#include <firmware/ring_buffer/ring_buffer.h>
#include <gtest/gtest.h>

TEST(RingBuffer, getRingBufferCount)
{
  RingBuffer buffer;
  clearRingBuffer(&buffer);

  ASSERT_EQ(0, getRingBufferCount(&buffer));

  for(size_t i = 0; i < 68; ++i)
  {
    putSymbolToRingBuffer(i, &buffer);
  }

  ASSERT_EQ(68, getRingBufferCount(&buffer));
}

TEST(RingBuffer, testThreshold)
{
  RingBuffer buffer;
  clearRingBuffer(&buffer);

  for(size_t i = 0; i < BUF_SIZE; ++i)
  {
    putSymbolToRingBuffer(i, &buffer);
  }

  ASSERT_EQ(0, getRingBufferCount(&buffer));
}

TEST(RingBuffer, testRingProperty)
{
  RingBuffer buffer;
  clearRingBuffer(&buffer);

  for(size_t i = 0; i < BUF_SIZE - 1; ++i)
    putSymbolToRingBuffer(i, &buffer);

  ASSERT_EQ(BUF_SIZE - 1, getRingBufferCount(&buffer));

  for(size_t i = 0; i < BUF_SIZE - 1; ++i)
  {
    ASSERT_EQ(i, getSymbolFromRingBuffer(&buffer));
    putSymbolToRingBuffer(i, &buffer);
    ASSERT_EQ(BUF_SIZE - 1, getRingBufferCount(&buffer));
  }
}

TEST(RingBuffer, testRemoveNonexistentSymbol)
{
  RingBuffer buffer;
  clearRingBuffer(&buffer);

  removeSymbolsFromRingBuffer(1, &buffer);

  //! \bug Так быть не должно - ожидаемо иметь 0
  ASSERT_EQ(255, getRingBufferCount(&buffer));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

/* ===[ End of file  ]=== */
