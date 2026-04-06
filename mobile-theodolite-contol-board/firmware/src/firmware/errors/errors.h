/*
 * errors.h
 *
 *      Author: mmalenko
 *  (c) 2015 NIC SPbETU
 */

#ifndef FIRMWARE_ERRORS_ERRORS_H_
#define FIRMWARE_ERRORS_ERRORS_H_

#include "firmware/queue/queue.h"

template<typename ErrorType>
class ErrorsStorage
{
public:

  ErrorsStorage():
    m_errorsQueue(10)
  {
  }

  void sendError(ErrorType error)
  {
    m_errorsQueue.put(error);
  }

  uint16_t getErrorCount()
  {
    return m_errorsQueue.getCount();
  }

  ErrorType getError()
  {
    return m_errorsQueue.get();
  }

private:
  Queue<ErrorType> m_errorsQueue;
};


#endif /* FIRMWARE_ERRORS_ERRORS_H_ */

