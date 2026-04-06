/*
 * gpio.h
 *
 *      Author: mmalenko
 *  (c) 2016 NIC SPbETU
 */

#ifndef LOGIC_GPIO_GPIO_H_
#define LOGIC_GPIO_GPIO_H_

class GpioPinInterface
{
public:
  virtual ~GpioPinInterface()
  {

  }
  virtual void setHigh()=0;
  virtual void setLow()=0;
  virtual bool getLevel()=0;
  virtual void setLevel(bool)=0;
};

class GpioPinInterfaceDummy:
  public GpioPinInterface
{
  bool m_level;
  public:
  virtual ~GpioPinInterfaceDummy()
  {

  }
  GpioPinInterfaceDummy()
  {
    m_level = false;
  }
  void setHigh()
  {
    m_level = true;
  }
  void setLow()
  {
    m_level = false;
  }
  bool getLevel()
  {
    return m_level;
  }
  void setLevel(bool level)
  {
    m_level = level;
  }
};

typedef enum
{
  MANUAL_CNT_AZIMUTH_INCREMENT_PIN,
  MANUAL_CNT_AZIMUTH_DECREMENT_PIN,
  MANUAL_CNT_ELEVATION_INCREMENT_PIN,
  MANUAL_CNT_ELEVATION_DECREMENT_PIN,
  MANUAL_CONTROLLER_CONNECT_SENSE_PIN,
  SEND_REPLY_LED_PIN,
  RECEIVE_COMMAND_LED_PIN,
  CAMERA_TX_TEST_PIN,
  AZIMUTH_BRAKE_PIN,
  ELEVATION_BRAKE_PIN,
  SYNC_SELECTOR_PIN,
  CAMERA_TX_SOFT_PIN
} GpioPins;

GpioPinInterface* getPin(GpioPins pinName);
void gpioConfiguration();

#endif /* PERIPHERAL_GPIO_GPIO_H_ */
