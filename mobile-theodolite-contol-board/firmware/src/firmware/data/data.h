///*
// * data.h
// *
// *  Created on: 29 нояб. 2018 г.
// *      Author: mmalenko
// *  (c) 2018 NIC SPbETU
// */

#ifndef FIRMWARE_DATA_DATA_H_
#define FIRMWARE_DATA_DATA_H_

#include <stdint.h>
#include <stddef.h>

template<typename T>
struct Data
{
  T *m_data;
  size_t m_size;
  Data(T *data,
       size_t size) :
    m_data(data), m_size(size)
  {

  }
};

#endif /* FIRMWARE_DATA_DATA_H_ */
