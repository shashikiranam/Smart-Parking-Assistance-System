/**
 ******************************************************************************
 * @file    utilities.h
 * @brief   Small generic helper functions shared across modules
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Clamp a float value into an inclusive [min, max] range.
 */
float util_clampf(float value, float min, float max);

/**
 * @brief Clamp an unsigned 32-bit value into an inclusive [min, max] range.
 */
uint32_t util_clamp_u32(uint32_t value, uint32_t min, uint32_t max);

/**
 * @brief Linear interpolation: maps `value` from [in_min, in_max] to
 *        [out_min, out_max]. Result is NOT clamped.
 */
float util_map_float(float value, float in_min, float in_max,
                      float out_min, float out_max);

/**
 * @brief Returns the smaller of two float values.
 */
float util_min_f(float a, float b);

/**
 * @brief Returns the larger of two float values.
 */
float util_max_f(float a, float b);

/**
 * @brief Simple debounce helper. Call periodically with the raw pin state;
 *        returns true once the state has been stable for `stable_ms`.
 *
 * @param raw_state       Current raw (noisy) logic level.
 * @param stable_ms       Required stable duration in milliseconds.
 * @param tick_ms         Current millisecond tick (monotonic).
 * @param p_last_state    Persistent storage for last observed raw state.
 * @param p_last_change_ms Persistent storage for the tick of last change.
 * @param p_debounced_state Persistent storage for the debounced output.
 * @return true if p_debounced_state changed on this call.
 */
bool util_debounce(bool raw_state, uint32_t stable_ms, uint32_t tick_ms,
                    bool *p_last_state, uint32_t *p_last_change_ms,
                    bool *p_debounced_state);

/**
 * @brief Formats a float distance value as "%d.%01d" style integer/decimal
 *        pair without relying on printf's %f (keeps UART/LCD code small and
 *        avoids pulling in the full floating point printf library).
 *
 * @param value       Input value, e.g. 45.7f
 * @param p_int_part  Output integer part, e.g. 45
 * @param p_dec_part  Output single decimal digit part, e.g. 7
 */
void util_split_decimal(float value, int32_t *p_int_part, uint32_t *p_dec_part);

#ifdef __cplusplus
}
#endif

#endif /* UTILITIES_H */
