/**
 ******************************************************************************
 * @file    uart_debug.h
 * @brief   UART debug telemetry output (sensor readings, state, decisions)
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#ifndef UART_DEBUG_H
#define UART_DEBUG_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32g0xx_hal.h"
#include "decision_engine.h"
#include "system_state.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the UART debug module and print a startup banner.
 */
void uart_debug_init(UART_HandleTypeDef *huart);

/**
 * @brief Transmit one line of free-form text followed by CRLF.
 */
void uart_debug_print(const char *line);

/**
 * @brief Transmit a full telemetry frame: the four filtered sensor
 *        distances, current FSM state, parking severity, and the
 *        recommended action. Format (single line, CRLF terminated):
 *
 *   FL:45.2cm FR:70.1cm RL:22.0cm RR:90.4cm STATE:SENSOR_SCAN SEVERITY:WARNING ACTION:TurnRight
 */
void uart_debug_report(const decision_input_t *input,
                        const decision_output_t *output,
                        fsm_state_t fsm_state);

#ifdef __cplusplus
}
#endif

#endif /* UART_DEBUG_H */
