/**
 ******************************************************************************
 * @file    parking.h
 * @brief   Application-layer orchestration: sensors -> filter -> decision
 *          -> outputs (LCD/buzzer/LEDs/UART). Implements the FSM step
 *          functions dispatched from main.c.
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#ifndef PARKING_H
#define PARKING_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32g0xx_hal.h"
#include "system_state.h"
#include "filter.h"
#include "decision_engine.h"
#include "ultrasonic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    I2C_HandleTypeDef  *hi2c_lcd;
    UART_HandleTypeDef *huart_debug;
    TIM_HandleTypeDef  *htim_micros;   /* free-running us timer for echo timing */
    TIM_HandleTypeDef  *htim_servo;
    uint32_t             servo_channel;
} parking_hw_handles_t;

typedef struct
{
    system_state_t      fsm;
    parking_hw_handles_t hw;

    filter_state_t       filt_fl;
    filter_state_t       filt_fr;
    filter_state_t       filt_rl;
    filter_state_t       filt_rr;

    decision_input_t      last_input;
    decision_output_t     last_output;

    uint32_t              last_scan_tick_ms;
    uint32_t              last_lcd_update_tick_ms;
    uint32_t              last_uart_update_tick_ms;

    bool                   button_last_raw;
    bool                   button_debounced;
    uint32_t               button_last_change_ms;
} parking_app_t;

/**
 * @brief One-time application initialization. Configures all driver
 *        sub-modules (ultrasonic, servo, LCD, buzzer, LED) and resets the
 *        FSM to SYSTEM_INIT.
 */
void parking_app_init(parking_app_t *app, const parking_hw_handles_t *hw);

/**
 * @brief Runs the power-on self test: verifies each ultrasonic sensor
 *        returns a plausible reading and the LCD acknowledges over I2C.
 *        Transitions to SENSOR_SCAN on success or ERROR on failure/timeout.
 */
void parking_app_run_self_test(parking_app_t *app);

/**
 * @brief Executes exactly one FSM step based on app->fsm.current_state.
 *        Intended to be called every SYSTEM_TICK_MS from the main loop.
 */
void parking_app_run(parking_app_t *app);

/**
 * @brief Call from the EXTI callback for the parking-mode push button.
 */
void parking_app_button_isr(parking_app_t *app);

/**
 * @brief Call from HAL_SYSTICK_Callback (or a dedicated 1ms timer ISR) to
 *        advance the FSM's millisecond tick counter.
 */
void parking_app_tick_isr(parking_app_t *app);

#ifdef __cplusplus
}
#endif

#endif /* PARKING_H */
