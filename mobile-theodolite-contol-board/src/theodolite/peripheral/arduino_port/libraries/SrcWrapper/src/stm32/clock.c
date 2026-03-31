/*
 *******************************************************************************
 * Copyright (c) 2016-2021, STMicroelectronics
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Function called to read the current micro second
 * @param  None
 * @retval None
 */
uint32_t getCurrentMicros(void)
{
  uint32_t m0 = HAL_GetTick();
  __IO uint32_t u0 = MKS_TIM->CNT;
  uint32_t m1 = HAL_GetTick();
  __IO uint32_t u1 = MKS_TIM->CNT;
  if (m1 != m0)
    return m1 * 1000 + u1;
  else
    return m0 * 1000 + u0;

}

/**
 * @brief  Function called wto read the current millisecond
 * @param  None
 * @retval None
 */
uint32_t getCurrentMillis(void)
{
  return HAL_GetTick();
}

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
