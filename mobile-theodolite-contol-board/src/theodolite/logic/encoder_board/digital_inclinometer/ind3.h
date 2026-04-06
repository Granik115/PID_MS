/*
 * ind3.h
 *
 *  Created on: Jan 15, 2025
 *      Author: mvm
 */

#ifndef THEODOLITE_LOGIC_ENCODER_BOARD_DIGITAL_INCLINOMETER_IND3_H_
#define THEODOLITE_LOGIC_ENCODER_BOARD_DIGITAL_INCLINOMETER_IND3_H_

#include "stdint.h"
#include "encoder_board/inclinometer/inclinometer_interface.h"
#include "firmware/interfaces/interfaces.h"
#include "soft_timer/sw_timer.h"

const uint8_t IND3_ADDRESS = 1;

#pragma pack(push,1)
typedef enum
{
  INC_DIMENSION_SEC = 0,
  INC_DIMENSION_MIN = 1
} IncDimension;

typedef enum
{
  INC_SIGN_POSITIVE = 0,
  INC_SIGN_NEGATIVE = 1
} IncSign;

typedef union
{
  struct
  {
    uint16_t integer :14;
    IncDimension dimension :1;
    IncSign sign :1;
  } measure;
  uint16_t data;
} AxisIntegerData;

typedef struct
{
  uint8_t fractionPart;
  AxisIntegerData integerData;
} AxisMeasure;

typedef struct
{
  uint8_t protocolId;
  uint8_t requiestType;
  uint8_t address;
} InD3MessageHeader;

typedef struct
{
  InD3MessageHeader header;
  AxisMeasure y_measure;
  AxisMeasure x_measure;
  uint8_t crc;
} InD3MeasureMessage;
#pragma pack(pop)

class InD3Inclinometer:
  public InclinometerInterface
{
public:
  virtual ~InD3Inclinometer()
  {

  }
  InD3Inclinometer(ConnectionInterface &interface, uint8_t address);
  void update();
  int32_t getXMills();
  int32_t getYMills();
  bool isDataReliable();
  private:
  enum ProtocolCommands
  {
    GET_MEASURE_ID = 0x01
  };
  const uint8_t REQUEST_MEASURE_DATA_LEN = 4;
  const uint8_t FRAME_DELIMITER = 0x7e;
  const uint8_t ESCAPE_BYTE = 0x7d;
  const uint8_t ESCAPE_MASK = 1 << 5;
  const uint8_t PROTOCOL_ID_MAIN = 0x9b;
  const uint8_t PROTOCOL_ID_AUX = 0x9c;
  const int32_t DATA_RELIABLE_TIMEOUT = 3000;
  static const uint8_t PACKET_BUFFER_MASK = 0x1F;
  int32_t m_xmills;
  int32_t m_ymills;
  bool m_isDataReliable;
  ConnectionInterface &m_interface;
  uint8_t m_address;
  Timer m_dataReliableTimer;
  uint8_t m_packetBuffer[PACKET_BUFFER_MASK + 1];
  uint8_t m_packetBufferPointer;
  bool isStartDetect;
  size_t receiveFrame(uint8_t byte);
  uint32_t getEscapeCount(uint8_t *dataIn, size_t dataInLen);
  int32_t getFrameDelimeterIndex(uint8_t *dataIn, size_t dataInLen);
  bool decodeMessage(uint8_t *packetBuffer, size_t len);
  size_t decodeEscaping(uint8_t *dataIn, size_t dataInLen, uint8_t *dataOut);
  size_t encodeEscaping(uint8_t *dataIn, size_t dataInLen, uint8_t *dataOut);
  size_t encodeFrame(uint8_t *dataIn, size_t dataInLen, uint8_t *dataOut);
  void updateMeasures(InD3MeasureMessage*);
  uint8_t crc(uint8_t *dataIn, size_t dataInLen);
  int32_t getMills(AxisMeasure measure);
  InD3MeasureMessage* decodeMeasureMessage(uint8_t *data, size_t size);
  bool sendMeasureDataRequest();
};

ConnectionInterface& getInclinometerConnectionInterface();

#endif /* THEODOLITE_LOGIC_ENCODER_BOARD_DIGITAL_INCLINOMETER_IND3_H_ */
