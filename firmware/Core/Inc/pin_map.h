/**
 ******************************************************************************
 * @file    pin_map.h
 * @brief   Central pin/peripheral assignment map for NUCLEO-G071RB
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 * All physical pin assignments are defined here ONLY. Application and driver
 * modules must never hardcode a GPIO port/pin directly; they must include
 * this header instead. This keeps the board support layer swappable.
 *
 * NOTE ON EXTI LINES: on STM32, EXTI lines 0-15 are shared across GPIO ports
 * (only one port can be routed to a given EXTI line number at a time via
 * SYSCFG). The four ultrasonic ECHO pins below were deliberately assigned
 * to four DIFFERENT pin numbers (1, 8, 11, 12) so all four can be used as
 * independent external interrupt sources simultaneously.
 ******************************************************************************
 */

#ifndef PIN_MAP_H
#define PIN_MAP_H

#include "stm32g0xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==========================================================================
 *  ULTRASONIC SENSORS (HC-SR04) - Trigger (output) / Echo (EXTI input)
 * ==========================================================================
 *  Echo pulse width is measured via EXTI rising/falling edge interrupts
 *  timestamped against a free-running microsecond timer (see ultrasonic.c).
 * ==========================================================================*/

/* Front Left */
#define US_FL_TRIG_PORT                 GPIOA
#define US_FL_TRIG_PIN                  GPIO_PIN_0
#define US_FL_ECHO_PORT                 GPIOA
#define US_FL_ECHO_PIN                  GPIO_PIN_1     /* EXTI1 */

/* Front Right */
#define US_FR_TRIG_PORT                 GPIOA
#define US_FR_TRIG_PIN                  GPIO_PIN_4
#define US_FR_ECHO_PORT                 GPIOA
#define US_FR_ECHO_PIN                  GPIO_PIN_8     /* EXTI8 */

/* Rear Left */
#define US_RL_TRIG_PORT                 GPIOB
#define US_RL_TRIG_PIN                  GPIO_PIN_0
#define US_RL_ECHO_PORT                 GPIOB
#define US_RL_ECHO_PIN                  GPIO_PIN_11    /* EXTI11 */

/* Rear Right */
#define US_RR_TRIG_PORT                 GPIOB
#define US_RR_TRIG_PIN                  GPIO_PIN_10
#define US_RR_ECHO_PORT                 GPIOB
#define US_RR_ECHO_PIN                  GPIO_PIN_12    /* EXTI12 */

/* ==========================================================================
 *  SERVO (SG90) - PWM output
 * ==========================================================================*/
#define SERVO_PWM_PORT                  GPIOA
#define SERVO_PWM_PIN                   GPIO_PIN_6     /* TIM3_CH1 */

/* ==========================================================================
 *  BUZZER (Piezo) - PWM/GPIO output
 * ==========================================================================*/
#define BUZZER_PORT                     GPIOA
#define BUZZER_PIN                      GPIO_PIN_7     /* TIM14_CH1 or GPIO */

/* ==========================================================================
 *  LEDs
 * ==========================================================================*/
#define LED_GREEN_PORT                  GPIOB
#define LED_GREEN_PIN                   GPIO_PIN_3

#define LED_RED_PORT                    GPIOB
#define LED_RED_PIN                     GPIO_PIN_4

/* On-board user LED (LD4) kept for heartbeat / self-test indication */
#define LED_HEARTBEAT_PORT              GPIOA
#define LED_HEARTBEAT_PIN               GPIO_PIN_5

/* ==========================================================================
 *  PUSH BUTTON - Parking mode enable/disable
 * ==========================================================================*/
#define BTN_PARKING_MODE_PORT           GPIOC
#define BTN_PARKING_MODE_PIN            GPIO_PIN_13    /* On-board B1, EXTI13 */

/* ==========================================================================
 *  I2C (LCD 16x2 via PCF8574 backpack)
 * ==========================================================================*/
#define LCD_I2C_SCL_PORT                GPIOB
#define LCD_I2C_SCL_PIN                 GPIO_PIN_8     /* I2C1_SCL */
#define LCD_I2C_SDA_PORT                GPIOB
#define LCD_I2C_SDA_PIN                 GPIO_PIN_9     /* I2C1_SDA */

/* ==========================================================================
 *  UART DEBUG (ST-LINK Virtual COM Port)
 * ==========================================================================*/
#define UART_DEBUG_TX_PORT              GPIOA
#define UART_DEBUG_TX_PIN               GPIO_PIN_2     /* USART2_TX (VCP) */
#define UART_DEBUG_RX_PORT              GPIOA
#define UART_DEBUG_RX_PIN               GPIO_PIN_3     /* USART2_RX (VCP) */

/* ==========================================================================
 *  FREE-RUNNING MICROSECOND TIMER (used for echo pulse timing + servo dwell)
 * ==========================================================================*/
#define MICROS_TIMER_INSTANCE           TIM2           /* 1 MHz free-running */

#ifdef __cplusplus
}
#endif

#endif /* PIN_MAP_H */
