///*(c) 2018 NIC SPbETU
// * serial_protocol_test.cpp
// *
// *  Created on: 15 авг. 2018 г.
// *      Author: maxim
// */

#include <gtest/gtest.h>

uint16_t turnerPositionRequestData[] =
{0xA0, 0, 0, 0, 0, 0, 0, 0};

uint16_t turnerPositionRequestWrongData[] =
{0xA0, 0, 0, 1, 0, 0x45, 0, 0};

uint16_t turnerPositionReplyData[] =
{0x50, 0x12, 0x34, 0x56, 0x12, 0x36, 0x54, 0x01, 0x02, 0x3, 0x4, 0};

const uint16_t REQUEST_CMD_LEN = 8;
const uint32_t POSITION_REQUEST_MARKER = 0xA0;
const uint16_t POSITION_REPLY_MARKER = 0x50;
const uint16_t POSITION_REPLY_LEN = 12;
const uint16_t ROTATE_COORDINATES_MARKER = 0xA1;
const uint16_t ROTATE_COORDINATES_ACK_MARKER = 0x51;
const uint16_t CAMERA_SETTINGS_MARKER = 0xA2;
const uint16_t CAMERA_SETTINGS_ACK_MARKER = 0x52;
const uint16_t BLOCKING_STATE_REQUEST_MARKER = 0xA3;
const uint16_t BLOCKING_STATE_MARKER = 0x53;
const uint16_t BLOCKING_STATE_LEN = 4;

TEST(SerialProtocolTest, ParseTurnerPositionRequest)
{

}
