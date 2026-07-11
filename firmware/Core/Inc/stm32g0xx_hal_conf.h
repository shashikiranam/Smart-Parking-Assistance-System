/**
 ******************************************************************************
 * @file    stm32g0xx_hal_conf.h
 * @brief   HAL configuration file - enables the peripheral modules used by
 *          this project and defines core clock/timeout parameters.
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 * @attention
 * This file configures which parts of the STM32G0xx HAL driver library are
 * compiled in. It expects Drivers/STM32G0xx_HAL_Driver and Drivers/CMSIS to
 * be present (added via STM32CubeMX / the STM32G0 firmware package - see
 * firmware/Drivers/README.md for instructions). Those driver sources are
 * maintained by STMicroelectronics and are intentionally NOT duplicated in
 * this repository.
 ******************************************************************************
 */

#ifndef STM32G0xx_HAL_CONF_H
#define STM32G0xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==========================================================================
 *  MODULE SELECTION
 * ==========================================================================*/
#define HAL_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_EXTI_MODULE_ENABLED
#define HAL_IWDG_MODULE_ENABLED

/* ==========================================================================
 *  OSCILLATOR VALUES
 * ==========================================================================*/
#if !defined(HSE_VALUE)
#define HSE_VALUE 8000000U
#endif

#if !defined(HSI_VALUE)
#define HSI_VALUE 16000000U
#endif

#if !defined(LSE_VALUE)
#define LSE_VALUE 32768U
#endif

#if !defined(LSI_VALUE)
#define LSI_VALUE 32000U
#endif

/* ==========================================================================
 *  SYSTEM CONFIGURATION
 * ==========================================================================*/
#define VDD_VALUE                    3300U
#define TICK_INT_PRIORITY            0U
#define USE_RTOS                     0U
#define PREFETCH_ENABLE               1U

#define USE_HAL_TIM_REGISTER_CALLBACKS   0U
#define USE_HAL_I2C_REGISTER_CALLBACKS   0U
#define USE_HAL_UART_REGISTER_CALLBACKS  0U

/* ==========================================================================
 *  ASSERT CONFIGURATION
 * ==========================================================================*/
/* #define USE_FULL_ASSERT    1U */

#ifdef USE_FULL_ASSERT
#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif

#include "stm32g0xx_hal_rcc.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_dma.h"
#include "stm32g0xx_hal_cortex.h"
#include "stm32g0xx_hal_tim.h"
#include "stm32g0xx_hal_i2c.h"
#include "stm32g0xx_hal_uart.h"
#include "stm32g0xx_hal_flash.h"
#include "stm32g0xx_hal_pwr.h"
#include "stm32g0xx_hal_exti.h"
#include "stm32g0xx_hal_iwdg.h"

#ifdef __cplusplus
}
#endif

#endif /* STM32G0xx_HAL_CONF_H */
