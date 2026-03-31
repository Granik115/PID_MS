/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_crc.h"
#include "stm32f4xx_ll_iwdg.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_rng.h"
#include "stm32f4xx_ll_rtc.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_wwdg.h"
#include "stm32f4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#define MKS_TIM TIM14 //TIM14 is HAL timebase tim. it's important for getCurrentMicros()
#define EL_TIM TIM8
#define AZ_TIM TIM1
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC1_SYNC_TIM TIM4
#define UM_PWM_BUS_ENABLE_Pin LL_GPIO_PIN_13
#define UM_PWM_BUS_ENABLE_GPIO_Port GPIOC
#define AZ_PWM_BUS_ENABLE_Pin LL_GPIO_PIN_14
#define AZ_PWM_BUS_ENABLE_GPIO_Port GPIOC
#define PWM_M2_AN_Pin LL_GPIO_PIN_5
#define PWM_M2_AN_GPIO_Port GPIOA
#define GNSS_PPS_Pin LL_GPIO_PIN_6
#define GNSS_PPS_GPIO_Port GPIOA
#define MII_RST_Pin LL_GPIO_PIN_0
#define MII_RST_GPIO_Port GPIOG
#define BRAKE_AZ_Pin LL_GPIO_PIN_7
#define BRAKE_AZ_GPIO_Port GPIOE
#define PWM_M1_AN_Pin LL_GPIO_PIN_8
#define PWM_M1_AN_GPIO_Port GPIOE
#define PWM_M1_A_Pin LL_GPIO_PIN_9
#define PWM_M1_A_GPIO_Port GPIOE
#define PWM_M1_BN_Pin LL_GPIO_PIN_10
#define PWM_M1_BN_GPIO_Port GPIOE
#define PWM_M1_B_Pin LL_GPIO_PIN_11
#define PWM_M1_B_GPIO_Port GPIOE
#define PWM_M1_CN_Pin LL_GPIO_PIN_12
#define PWM_M1_CN_GPIO_Port GPIOE
#define PWM_M1_C_Pin LL_GPIO_PIN_13
#define PWM_M1_C_GPIO_Port GPIOE
#define SYNC_OUT_Pin LL_GPIO_PIN_11
#define SYNC_OUT_GPIO_Port GPIOB
#define PWM_M2_BN_Pin LL_GPIO_PIN_14
#define PWM_M2_BN_GPIO_Port GPIOB
#define PWM_M2_CN_Pin LL_GPIO_PIN_15
#define PWM_M2_CN_GPIO_Port GPIOB
#define CAMERA_TX_Pin LL_GPIO_PIN_8
#define CAMERA_TX_GPIO_Port GPIOD
#define CAMERA_RX_Pin LL_GPIO_PIN_9
#define CAMERA_RX_GPIO_Port GPIOD
#define TRIGGER_Pin LL_GPIO_PIN_13
#define TRIGGER_GPIO_Port GPIOD
#define UM_SSI_DATA_Pin LL_GPIO_PIN_3
#define UM_SSI_DATA_GPIO_Port GPIOG
#define UM_SSI_CLK_Pin LL_GPIO_PIN_4
#define UM_SSI_CLK_GPIO_Port GPIOG
#define AZ_SSI_DATA_Pin LL_GPIO_PIN_5
#define AZ_SSI_DATA_GPIO_Port GPIOG
#define AZ_SSI_CLK_Pin LL_GPIO_PIN_6
#define AZ_SSI_CLK_GPIO_Port GPIOG
#define PWM_M2_A_Pin LL_GPIO_PIN_6
#define PWM_M2_A_GPIO_Port GPIOC
#define PWM_M2_B_Pin LL_GPIO_PIN_7
#define PWM_M2_B_GPIO_Port GPIOC
#define PWM_M2_C_Pin LL_GPIO_PIN_8
#define PWM_M2_C_GPIO_Port GPIOC
#define BRAKE_UM_Pin LL_GPIO_PIN_9
#define BRAKE_UM_GPIO_Port GPIOC
#define SEV_RX_Pin LL_GPIO_PIN_10
#define SEV_RX_GPIO_Port GPIOA
#define SEV_PPS_Pin LL_GPIO_PIN_15
#define SEV_PPS_GPIO_Port GPIOA
#define INC_UART_TX_Pin LL_GPIO_PIN_10
#define INC_UART_TX_GPIO_Port GPIOC
#define INC_UART_RX_Pin LL_GPIO_PIN_11
#define INC_UART_RX_GPIO_Port GPIOC
#define GYRO_UART_TX_Pin LL_GPIO_PIN_12
#define GYRO_UART_TX_GPIO_Port GPIOC
#define GYRO_UART_RX_Pin LL_GPIO_PIN_2
#define GYRO_UART_RX_GPIO_Port GPIOD
#define GNSS_TX_Pin LL_GPIO_PIN_5
#define GNSS_TX_GPIO_Port GPIOD
#define GNSS_RX_Pin LL_GPIO_PIN_6
#define GNSS_RX_GPIO_Port GPIOD
#define TEST_LED_Pin LL_GPIO_PIN_9
#define TEST_LED_GPIO_Port GPIOG
#define IMAGE_BOARD_CS_Pin LL_GPIO_PIN_6
#define IMAGE_BOARD_CS_GPIO_Port GPIOB
#define ENC_ETH_CS_Pin LL_GPIO_PIN_7
#define ENC_ETH_CS_GPIO_Port GPIOB
#define ENC_ETH_INT_Pin LL_GPIO_PIN_8
#define ENC_ETH_INT_GPIO_Port GPIOB
#define ENC_ETH_INT_EXTI_IRQn EXTI9_5_IRQn
#define ENC_ETH_RST_Pin LL_GPIO_PIN_9
#define ENC_ETH_RST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
/* Alias word address of Azimuth encoder DATA gpio pin */
#define AZ_SSI_DATA_Pin_POS           5
#define AZ_SSI_CLK_Pin_POS            6
#define UM_SSI_DATA_Pin_POS           3
#define UM_SSI_CLK_Pin_POS            4
#define AZIMUTH_SSI_DATA_BB           (PERIPH_BB_BASE + (((uint32_t)(&AZ_SSI_DATA_GPIO_Port->IDR) - PERIPH_BASE) * 32U) + (AZ_SSI_DATA_Pin_POS * 4U))
#define AZIMUTH_SSI_CLK_BB            (PERIPH_BB_BASE + (((uint32_t)(&AZ_SSI_CLK_GPIO_Port->ODR) - PERIPH_BASE) * 32U) + (AZ_SSI_CLK_Pin_POS * 4U))
#define ELEVATION_SSI_DATA_BB         (PERIPH_BB_BASE + (((uint32_t)(&UM_SSI_DATA_GPIO_Port->IDR) - PERIPH_BASE) * 32U) + (UM_SSI_DATA_Pin_POS * 4U))
#define ELEVATION_SSI_CLK_BB          (PERIPH_BB_BASE + (((uint32_t)(&UM_SSI_CLK_GPIO_Port->ODR) - PERIPH_BASE) * 32U) + (UM_SSI_CLK_Pin_POS * 4U))
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
