/*
 * assert_param.h
 *
 *  Created on: 23 мар. 2021 г.
 *      Author: maxim
 */

#ifndef THIRD_PARTY_MEDIASERVER_INCLUDE_ASSERT_H_
#define THIRD_PARTY_MEDIASERVER_INCLUDE_ASSERT_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include "stdint.h"
/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  *         If expr is true, it returns no value.
  * @retval None
  */
extern void assert_failed(uint8_t *file, uint32_t line);

  #define assert(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#ifdef  __cplusplus
}
#endif

#endif /* THIRD_PARTY_MEDIASERVER_INCLUDE_ASSERT_H_ */
