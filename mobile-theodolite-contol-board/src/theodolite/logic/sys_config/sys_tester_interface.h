///* (c) 2018 NIC SPbETU
// * sys_tester_interface.h
// *
// *  Created on: 16 июля 2018 г.
// *      Author: mmalenko
// */

#ifndef SRC_THEODOLITE_LOGIC_SYS_CONFIG_SYS_TESTER_INTERFACE_H_
#define SRC_THEODOLITE_LOGIC_SYS_CONFIG_SYS_TESTER_INTERFACE_H_

class SystemTesterInterface
{
public:
  virtual bool isCameraConnectionTestOk()=0;
  virtual bool isEncoderLoopbackTestOk()=0;
  virtual bool isOnboardFlashTestOk()=0;
  virtual bool isOnboardEepromTestOk()=0;
  virtual bool isExternalSyncTestOk()=0;
  virtual ~SystemTesterInterface()
  {

  }
};

void sysTesterConfig();
SystemTesterInterface *getSystemTester();

#endif /* SRC_THEODOLITE_LOGIC_SYS_CONFIG_SYS_TESTER_INTERFACE_H_ */
