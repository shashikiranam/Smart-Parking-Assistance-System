/**
 ******************************************************************************
 * @file    utilities.c
 * @brief   Small generic helper function implementations
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#include "utilities.h"
#include <math.h>

float util_clampf(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

uint32_t util_clamp_u32(uint32_t value, uint32_t min, uint32_t max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float util_map_float(float value, float in_min, float in_max,
                      float out_min, float out_max)
{
    float in_span  = in_max - in_min;
    float out_span = out_max - out_min;

    if (fabsf(in_span) < 1e-6f)
    {
        return out_min; /* avoid divide-by-zero on degenerate input range */
    }

    return out_min + ((value - in_min) * out_span) / in_span;
}

float util_min_f(float a, float b)
{
    return (a < b) ? a : b;
}

float util_max_f(float a, float b)
{
    return (a > b) ? a : b;
}

bool util_debounce(bool raw_state, uint32_t stable_ms, uint32_t tick_ms,
                    bool *p_last_state, uint32_t *p_last_change_ms,
                    bool *p_debounced_state)
{
    bool changed = false;

    if (raw_state != *p_last_state)
    {
        *p_last_state     = raw_state;
        *p_last_change_ms = tick_ms;
    }
    else if ((tick_ms - *p_last_change_ms) >= stable_ms)
    {
        if (*p_debounced_state != raw_state)
        {
            *p_debounced_state = raw_state;
            changed = true;
        }
    }

    return changed;
}

void util_split_decimal(float value, int32_t *p_int_part, uint32_t *p_dec_part)
{
    bool negative = (value < 0.0f);
    float abs_val = negative ? -value : value;

    int32_t int_part = (int32_t)abs_val;
    uint32_t dec_part = (uint32_t)((abs_val - (float)int_part) * 10.0f + 0.5f);

    if (dec_part >= 10U)
    {
        dec_part = 0U;
        int_part += 1;
    }

    *p_int_part = negative ? -int_part : int_part;
    *p_dec_part = dec_part;
}
