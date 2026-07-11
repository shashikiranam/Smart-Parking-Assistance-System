/**
 ******************************************************************************
 * @file    buzzer.c
 * @brief   Progressive piezo buzzer warning driver implementation
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#include "buzzer.h"
#include "pin_map.h"
#include "config.h"

static buzzer_pattern_t s_pattern       = BUZZER_PATTERN_OFF;
static uint32_t          s_period_start = 0U;
static bool               s_output_on    = false;

static void buzzer_write(bool on)
{
    s_output_on = on;
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void buzzer_init(void)
{
    buzzer_write(false);
    s_pattern = BUZZER_PATTERN_OFF;
    s_period_start = 0U;
}

void buzzer_set_pattern(buzzer_pattern_t pattern)
{
    if (pattern != s_pattern)
    {
        s_pattern = pattern;
        s_period_start = 0U; /* restart pattern timing cleanly on change */
    }
}

static uint32_t buzzer_period_for_pattern(buzzer_pattern_t pattern)
{
    switch (pattern)
    {
        case BUZZER_PATTERN_SLOW:   return BUZZER_PERIOD_SLOW_MS;
        case BUZZER_PATTERN_MEDIUM: return BUZZER_PERIOD_MEDIUM_MS;
        case BUZZER_PATTERN_FAST:   return BUZZER_PERIOD_FAST_MS;
        default:                    return 0U;
    }
}

void buzzer_update(uint32_t tick_ms)
{
    if (s_period_start == 0U)
    {
        s_period_start = tick_ms;
    }

    uint32_t elapsed = tick_ms - s_period_start;

    switch (s_pattern)
    {
        case BUZZER_PATTERN_OFF:
            buzzer_write(false);
            break;

        case BUZZER_PATTERN_CONTINUOUS:
            buzzer_write(true);
            break;

        case BUZZER_PATTERN_SLOW:
        case BUZZER_PATTERN_MEDIUM:
        case BUZZER_PATTERN_FAST:
        {
            uint32_t period = buzzer_period_for_pattern(s_pattern);
            uint32_t phase = elapsed % period;

            buzzer_write(phase < BUZZER_PULSE_ON_MS);
            break;
        }

        default:
            buzzer_write(false);
            break;
    }
}

void buzzer_silence(void)
{
    s_pattern = BUZZER_PATTERN_OFF;
    buzzer_write(false);
}
