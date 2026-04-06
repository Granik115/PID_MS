/*
 * sw_timer_mock.h
 *
 *  Created on: 11 авг. 2017 г.
 *      Author: mmalenko
 *  (c) 2017 NIC SPbETU
 */

#ifndef SW_TIMER_MOCK_H_
#define SW_TIMER_MOCK_H_

#include <theodolite/logic/soft_timer/sw_timer_interface.h>

class TimerMock:
  TimerInterface
{
public:
  MOCK_METHOD1(set,void(int32_t interval));
  MOCK_METHOD0(reset,void());
  MOCK_METHOD0(start,void());
  MOCK_METHOD0(stop,void());
  MOCK_METHOD0(isRunning,bool());
  MOCK_METHOD0(restart,void());
  MOCK_METHOD0(expired,bool());
  MOCK_METHOD0(remaining,int32_t());
};

#endif /* SW_TIMER_MOCK_H_ */
