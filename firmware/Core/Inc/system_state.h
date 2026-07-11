/**
 ******************************************************************************
 * @file    system_state.h
 * @brief   Finite State Machine (FSM) definitions for the main control loop
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Top level firmware finite state machine states.
 *
 * Flow (nominal path):
 *   SYSTEM_INIT -> SELF_TEST -> SENSOR_SCAN -> FILTER_DATA -> PROCESS_DATA
 *   -> DECISION_ENGINE -> DISPLAY_UPDATE -> WARNING_CONTROL -> UART_UPDATE
 *   -> SENSOR_SCAN (loop) ... -> IDLE (when parking mode disabled)
 *
 * ERROR is reachable from any state on a fault condition and can only be
 * exited by a successful re-run of SELF_TEST.
 */
typedef enum
{
    FSM_STATE_SYSTEM_INIT = 0,
    FSM_STATE_SELF_TEST,
    FSM_STATE_SENSOR_SCAN,
    FSM_STATE_FILTER_DATA,
    FSM_STATE_PROCESS_DATA,
    FSM_STATE_DECISION_ENGINE,
    FSM_STATE_DISPLAY_UPDATE,
    FSM_STATE_WARNING_CONTROL,
    FSM_STATE_UART_UPDATE,
    FSM_STATE_IDLE,
    FSM_STATE_ERROR,
    FSM_STATE_COUNT
} fsm_state_t;

/**
 * @brief Fault codes latched into system_state_t.last_error on ERROR entry.
 */
typedef enum
{
    FAULT_NONE = 0,
    FAULT_SELF_TEST_TIMEOUT,
    FAULT_SENSOR_FL_UNRESPONSIVE,
    FAULT_SENSOR_FR_UNRESPONSIVE,
    FAULT_SENSOR_RL_UNRESPONSIVE,
    FAULT_SENSOR_RR_UNRESPONSIVE,
    FAULT_I2C_LCD_NACK,
    FAULT_SERVO_OUT_OF_RANGE,
    FAULT_WATCHDOG_RESET
} fault_code_t;

/**
 * @brief Global mutable system state shared across modules.
 *
 * Ownership: written primarily by system_state.c / parking.c, read by all
 * other application-layer modules. Not written to from ISR context except
 * for the volatile fields explicitly marked below.
 */
typedef struct
{
    fsm_state_t   current_state;
    fsm_state_t   previous_state;
    fault_code_t  last_error;

    volatile bool parking_mode_enabled;   /* toggled from EXTI button ISR   */
    volatile uint32_t tick_ms;            /* incremented by SysTick, 1ms    */

    bool          self_test_passed;
    uint32_t      state_entry_tick;       /* tick_ms at last state entry    */
    uint32_t      fault_count;
} system_state_t;

/* ==========================================================================
 *  PUBLIC API
 * ==========================================================================*/
void        system_state_init(system_state_t *state);
void        system_state_transition(system_state_t *state, fsm_state_t next);
uint32_t    system_state_time_in_state_ms(const system_state_t *state);
void        system_state_raise_fault(system_state_t *state, fault_code_t code);
const char *system_state_name(fsm_state_t state);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_STATE_H */
