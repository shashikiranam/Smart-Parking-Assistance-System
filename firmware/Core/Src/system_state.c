/**
 ******************************************************************************
 * @file    system_state.c
 * @brief   Finite State Machine bookkeeping implementation
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#include "system_state.h"
#include <string.h>

void system_state_init(system_state_t *state)
{
    memset(state, 0, sizeof(*state));
    state->current_state         = FSM_STATE_SYSTEM_INIT;
    state->previous_state        = FSM_STATE_SYSTEM_INIT;
    state->last_error            = FAULT_NONE;
    state->parking_mode_enabled  = false;
    state->tick_ms                = 0U;
    state->self_test_passed      = false;
    state->state_entry_tick      = 0U;
    state->fault_count           = 0U;
}

void system_state_transition(system_state_t *state, fsm_state_t next)
{
    if (state == NULL || next >= FSM_STATE_COUNT)
    {
        return;
    }

    state->previous_state   = state->current_state;
    state->current_state    = next;
    state->state_entry_tick = state->tick_ms;
}

uint32_t system_state_time_in_state_ms(const system_state_t *state)
{
    if (state == NULL)
    {
        return 0U;
    }
    return (state->tick_ms - state->state_entry_tick);
}

void system_state_raise_fault(system_state_t *state, fault_code_t code)
{
    if (state == NULL)
    {
        return;
    }
    state->last_error = code;
    state->fault_count++;
    system_state_transition(state, FSM_STATE_ERROR);
}

const char *system_state_name(fsm_state_t state)
{
    switch (state)
    {
        case FSM_STATE_SYSTEM_INIT:      return "SYSTEM_INIT";
        case FSM_STATE_SELF_TEST:        return "SELF_TEST";
        case FSM_STATE_SENSOR_SCAN:      return "SENSOR_SCAN";
        case FSM_STATE_FILTER_DATA:      return "FILTER_DATA";
        case FSM_STATE_PROCESS_DATA:     return "PROCESS_DATA";
        case FSM_STATE_DECISION_ENGINE:  return "DECISION_ENGINE";
        case FSM_STATE_DISPLAY_UPDATE:   return "DISPLAY_UPDATE";
        case FSM_STATE_WARNING_CONTROL:  return "WARNING_CONTROL";
        case FSM_STATE_UART_UPDATE:      return "UART_UPDATE";
        case FSM_STATE_IDLE:             return "IDLE";
        case FSM_STATE_ERROR:            return "ERROR";
        default:                         return "UNKNOWN";
    }
}
