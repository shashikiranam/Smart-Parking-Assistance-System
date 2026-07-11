/**
 ******************************************************************************
 * @file    main.h
 * @brief   Application entry point declarations and global HAL handles
 * @project Smart-Parking-Assistance-System
 * @board   STM32 NUCLEO-G071RB (STM32G071RBT6)
 ******************************************************************************
 */

#ifndef MAIN_H
#define MAIN_H

#include "stm32g0xx_hal.h"
#include "pin_map.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==========================================================================
 *  GLOBAL PERIPHERAL HANDLES (defined in main.c, used by application layer)
 * ==========================================================================*/
extern I2C_HandleTypeDef  hi2c1;
extern TIM_HandleTypeDef  htim2;   /* free-running 1MHz microsecond timer */
extern TIM_HandleTypeDef  htim3;   /* servo PWM, 50Hz, channel 1 (PA6)     */
extern UART_HandleTypeDef huart2;  /* debug UART, ST-LINK VCP              */

/* ==========================================================================
 *  PUBLIC API
 * ==========================================================================*/
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
