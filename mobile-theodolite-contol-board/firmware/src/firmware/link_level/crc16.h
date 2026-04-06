///*
// * crc16.h
// *
// *      Author: mmalenko
// * (c) 2015 NIC SPbETU
// */

#ifndef INCLUDE_CRC16_H_
#define INCLUDE_CRC16_H_

#include <firmware/ring_buffer/ring_buffer.h>
#include <firmware/queue/fast_queue.h>

uint16_t crc16InRingBufferCalculate(RingBuffer *buf, uint16_t lenght,
                                    uint16_t firstSymbolNumber);
uint16_t crc16Calculate(QueueInterface<uint_least8_t> *buf, uint16_t lenght,
                        uint16_t firstSymbolNumber);
uint16_t crc16(uint_least8_t * pcBlock, uint32_t len);
uint16_t crc16UartLink(uint_least8_t marker, uint_least8_t * data,
                       uint_least8_t len);
const uint16_t* get16Table();
uint16_t crc16Word(uint16_t * pcBlock, uint32_t len);

#endif /* INCLUDE_CRC16_H_ */
