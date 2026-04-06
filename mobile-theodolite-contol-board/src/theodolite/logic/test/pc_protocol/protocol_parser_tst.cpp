/*
 * protocol_parser_tst.cpp
 *
 *      Author: mmalenko
 *  (c) 2016 NIC SPbETU
 */

#include <theodolite/logic/pc_protocol/protocol_processor.h>
#include <theodolite/logic/pc_protocol/third-party/theodolite_messages.pb.h>
#include <firmware/third-party/nanopb/pb_encode.h>
#include <firmware/third-party/nanopb/pb_decode.h>
#include <firmware/link_level/uart_link.h>
#include "commands_mock.h"
#include "axis_mover_mock.h"
#include "pid_reg_mock.h"
#include <gpio_mock.h>
#include <gtest/gtest.h>
#include <connection.h>
#include <firmware/errors/errors.h>

using ::testing::Return;

const uint_least8_t setAnglesDeltaStream[] =
{0xc0, 0x0d, 0x08, 0x01, 0x52,
  0x09, 0x0a, 0x07, 0x08, 0x90,
  0x4e, 0x10, 0xa0, 0x9c, 0x01,
  0xcf, 0x1b};

TEST(ParserTest, setAnglesDelta)
{
  ConnectionMock connection;
  ProtobufMessagesProcessor<theodolite_messages_CommandEnvelope> messageProcessor = ProtobufMessagesProcessor<
  theodolite_messages_CommandEnvelope>(connection,true,0xC0, theodolite_messages_CommandEnvelope_fields);
  FastQueue<uint_least8_t> *queue = connection.getRxQueue();

  for (uint16_t i = 0; i < sizeof(setAnglesDeltaStream); i++)
  {
    queue->put(setAnglesDeltaStream[i]);
  }
  theodolite_messages_CommandEnvelope message =
  { 0};

  ASSERT_EQ(messageProcessor.fillMessageFromUartLink(message), true);

  ASSERT_EQ(message.has_set_angles_delta, true);
}

TEST(ParserTest, sendMessage)
{
  /*
   MockCommands commandMock;
   TestInterface interface = TestInterface ();
   GpioPinMock pin1,pin2;
   ProtocolProcessor parser = ProtocolProcessor(&commandMock, &interface,&pin1,&pin2);
   theodolite_messages_CommandEnvelope message =
   { 0};
   message.has_rotate_to_coordinates_req = true;
   message.rotate_to_coordinates_req.turner_position.azimuth = 12345;
   message.rotate_to_coordinates_req.turner_position.elevation = 54321;
   parser.sendMessage (&message);
   Data data;
   uint_least8_t frameBuffer[MAX_FRAME_SIZE];
   data.data = frameBuffer;
   data.size = MAX_FRAME_SIZE;
   uint16_t receiveDataSize = getUartLinkData (interface.getTxRingBuffer (),
   &data);

   pb_istream_t stream = pb_istream_from_buffer (data.data, receiveDataSize);
   theodolite_messages_CommandEnvelope inMessage =
   { 0};

   bool status = pb_decode (&stream, theodolite_messages_CommandEnvelope_fields,
   &inMessage);

   ASSERT_EQ(status, true);
   ASSERT_EQ(inMessage.has_rotate_to_coordinates_req, true);
   ASSERT_EQ(inMessage.rotate_to_coordinates_req.turner_position.azimuth, 12345);
   ASSERT_EQ(inMessage.rotate_to_coordinates_req.turner_position.elevation,
   54321);*/
}

TEST(ParserTest, getMessage)
{
  /* MockCommands commandMock;
   TestInterface interface = TestInterface ();
   ProtocolProcessor parser = ProtocolProcessor (&commandMock, &interface);
   theodolite_messages_CommandEnvelope message =
   { 0};
   message.has_rotate_to_coordinates_req = true;
   message.rotate_to_coordinates_req.turner_position.azimuth = 12345;
   message.rotate_to_coordinates_req.turner_position.elevation = 54321;

   Data data;
   uint_least8_t frameBuffer[MAX_FRAME_SIZE];
   data.data = frameBuffer;
   data.size = MAX_FRAME_SIZE;
   pb_ostream_t stream = pb_ostream_from_buffer (frameBuffer,
   sizeof(frameBuffer));

   bool status = pb_encode (&stream, theodolite_messages_CommandEnvelope_fields,
   &message);

   ASSERT_EQ(status, true);

   uint_least8_t linkBuffer[MAX_FRAME_SIZE];
   data.size = stream.bytes_written;

   uint16_t writtenSize = uartLinkEncode (&data, linkBuffer);

   for (uint16_t i = 0; i < writtenSize; i++)
   {
   putSymbolToRingBuffer (linkBuffer[i], interface.getRxRingBuffer ());
   }

   theodolite_messages_CommandEnvelope inMessage =
   { 0};

   status = parser.getMessage (&inMessage);

   ASSERT_EQ(status, true);
   ASSERT_EQ(inMessage.has_rotate_to_coordinates_req, true);
   ASSERT_EQ(inMessage.rotate_to_coordinates_req.turner_position.azimuth, 12345);
   ASSERT_EQ(inMessage.rotate_to_coordinates_req.turner_position.elevation,
   54321);*/
}

TEST(ParserTest, SetRotateRequest)
{
  /*
   MockCommands commandMock;
   TestInterface interface = TestInterface ();
   ProtocolParser parser = ProtocolParser (&commandMock, &interface);
   theodolite_messages_CommandEnvelope message =
   { 0};
   message.has_rotate_to_coordinates_req = true;
   message.rotate_to_coordinates_req.turner_position.azimuth = 12345;
   message.rotate_to_coordinates_req.turner_position.elevation = 54321;
   Data data;
   uint_least8_t frameBuffer[MAX_FRAME_SIZE];
   data.data = frameBuffer;
   data.size = MAX_FRAME_SIZE;
   pb_ostream_t stream = pb_ostream_from_buffer (frameBuffer,
   sizeof(frameBuffer));

   bool status = pb_encode (&stream, theodolite_messages_CommandEnvelope_fields,
   &message);

   ASSERT_EQ(status, true);
   EXPECT_CALL(commandMock, rotateToCoordinates(12345,54321));

   uint_least8_t linkBuffer[MAX_FRAME_SIZE];
   data.size = stream.bytes_written;

   uint16_t writtenSize = uartLinkEncode (&data, linkBuffer);

   for (uint16_t i = 0; i < writtenSize; i++)
   {
   putSymbolToRingBuffer (linkBuffer[i], interface.getRxRingBuffer ());
   }

   status = parser.parse ();
   ASSERT_EQ(status, true);

   uint16_t txBytesCount = getRingBufferCount (interface.getTxRingBuffer ());

   ASSERT_GT(txBytesCount, 0);

   for (uint16_t i = 0; i < txBytesCount; i++)
   {
   uint_least8_t byte = getSymbolFromRingBuffer (
   interface.getTxRingBuffer ());
   putSymbolToRingBuffer (byte, interface.getRxRingBuffer ());
   }

   theodolite_messages_CommandEnvelope inMessage =
   { 0};
   status = parser.getMessage (&inMessage);
   ASSERT_EQ(status, true);
   ASSERT_EQ(inMessage.has_ack, true);*/
}

using ::testing::ReturnRef;

TEST(ParserTest, GetState)
{
  MockCommands commandMock;
  PidRegMock pidMock;
  TestInterface interface = TestInterface();
  GpioPinMock replyLed;
  GpioPinMock receiveLed;
  ProtocolProcessor parser = ProtocolProcessor(&commandMock, interface, &receiveLed, &replyLed);
  theodolite_messages_CommandEnvelope message =
  { 0};
  message.has_state_req = true;
  uint_least8_t frameBuffer[MAX_FRAME_SIZE];
  Data<uint_least8_t> data = Data<uint_least8_t>(frameBuffer, MAX_FRAME_SIZE);
  pb_ostream_t stream = pb_ostream_from_buffer(frameBuffer,
  sizeof(frameBuffer));

  bool status = pb_encode(&stream, theodolite_messages_CommandEnvelope_fields,
  &message);

  ASSERT_EQ(status, true);

  SystemState testState =
  { 0};
  testState.azimuthMills = 7;
  testState.azimuthBrake = true;
  testState.azimuthDriveActivity = true;
  testState.elevationMills = 1234567;
  testState.elevationBrake = false;
  testState.elevationDriveActivity = false;
  testState.exposure = 54321;
  testState.frequency = CameraFreq_FREQ_2;
  testState.gain = CameraGain_GAIN_6;

  testState.cameraExpositionType = CameraExpositionType_MANUAL;
  testState.cameraSyncType = CameraSyncType_ORIGIN;

  testState.inclinometerXMills = 33333;
  testState.inclinometerYMills = 44444;
  testState.state = axis_mover_interface::INITIALIZATION;

  AxisMoverMock axisMock;

  EXPECT_CALL(axisMock,getAxisPosition()).WillRepeatedly(Return(theodolite_messages_AxisPosition_UNKNOWN_POSITION));

  EXPECT_CALL(commandMock,getState()).WillOnce(Return(testState));
  EXPECT_CALL(commandMock,getAzimuthMoveProcessor()).WillRepeatedly(
  ReturnRef(axisMock));
  EXPECT_CALL(commandMock,getElevationMoveProcessor()).WillRepeatedly(
  ReturnRef(axisMock));
  EXPECT_CALL(axisMock,getRegulator()).WillRepeatedly(
  ReturnRef(pidMock));
  EXPECT_CALL(pidMock,getKi()).WillRepeatedly(Return(1));
  EXPECT_CALL(pidMock,getKc()).WillRepeatedly(Return(2));
  EXPECT_CALL(pidMock,getKp()).WillRepeatedly(Return(3));

  uint_least8_t linkBuffer[MAX_FRAME_SIZE];
  data.m_size = stream.bytes_written;

  uint16_t writtenSize = uartLinkEncode(&data, linkBuffer, true,0xC0);

  for (uint16_t i = 0; i < writtenSize; i++)
  {
    interface.getRxQueue()->put(linkBuffer[i]);
  }

  status = parser.parseIncomingMessage();
  ASSERT_EQ(status, true);

  uint16_t txBytesCount = interface.getTxQueue()->getCount();

  ASSERT_GT(txBytesCount, 0);

  /* for (uint16_t i = 0; i < txBytesCount; i++)
   {
   uint_least8_t byte = getSymbolFromRingBuffer(
   interface.getTxRingBuffer());
   putSymbolToRingBuffer(byte, interface.getRxRingBuffer());
   }

   theodolite_messages_CommandEnvelope inMessage =
   {0};
   status = parser.getMessage(&inMessage);
   ASSERT_EQ(status, true);
   ASSERT_EQ(inMessage.has_state, true);
   ASSERT_EQ(inMessage.state.blocking_mode.is_azimuth_blocked,
   testState.azimuthBrake);
   ASSERT_EQ(inMessage.state.blocking_mode.is_elevation_blocked,
   testState.elevationBrake);
   ASSERT_EQ(inMessage.state.turner_position.azimuth, testState.azimuth);
   ASSERT_EQ(inMessage.state.turner_position.elevation, testState.elevation);
   ASSERT_EQ(inMessage.state.inclinometer_parameters.x, testState.inclinometerX);
   ASSERT_EQ(inMessage.state.inclinometer_parameters.y, testState.inclinometerY);
   ASSERT_EQ(inMessage.state.camera_info[0].camera_settings.exposure,
   testState.exposure);
   ASSERT_EQ(inMessage.state.camera_info_count, 1);
   ASSERT_EQ(inMessage.state.elevation_drive_activity,
   testState.elevationDriveActivity);
   ASSERT_EQ(inMessage.state.azimuth_drive_activity,
   testState.azimuthDriveActivity);

   axis_mover_interface::StateType stateType;
   switch (inMessage.state.state)
   {
   case theodolite_messages_StateType_IDLE:
   stateType = axis_mover_interface::IDLE;
   break;
   case theodolite_messages_StateType_ROTATION:
   stateType = axis_mover_interface::ROTATION;
   break;
   case theodolite_messages_StateType_TESTING:
   stateType = axis_mover_interface::INITIALIZATION;
   break;
   case theodolite_messages_StateType_UNKNOWN_STATE:
   stateType = axis_mover_interface::INITIALIZATION;
   break;
   default:
   stateType = axis_mover_interface::IDLE;
   break;
   }
   ASSERT_EQ(stateType, testState.state);*/
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

