/**
 ******************************************************************************
 * @file    buzzer.h
 * @brief   Progressive piezo buzzer warning driver
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32g0xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Buzzer alert patterns, ordered from least to most urgent. Mirrors
 *        the parking_state_t severity levels 1:1.
 */
typedef enum
{
    BUZZER_PATTERN_OFF = 0,     /* > 100 cm: silent               */
    BUZZER_PATTERN_SLOW,        /* 60-100 cm: slow beep            */
    BUZZER_PATTERN_MEDIUM,      /* 30-60 cm: medium beep           */
    BUZZER_PATTERN_FAST,        /* 10-30 cm: fast beep             */
    BUZZER_PATTERN_CONTINUOUS   /* < 10 cm: continuous tone        */
} buzzer_pattern_t;

/**
 * @brief Initialize the buzzer GPIO (driven as a simple digital on/off
 *        output; timing/pattern generation is done in software).
 */
void buzzer_init(void);

/**
 * @brief Select the active alert pattern. Takes effect on the next call to
 *        buzzer_update().
 */
void buzzer_set_pattern(buzzer_pattern_t pattern);

/**
 * @brief Must be called periodically (every scheduler tick, ~10ms) to
 *        generate the timing for the currently selected pattern.
 *
 * @param tick_ms Current monotonic millisecond tick.
 */
void buzzer_update(uint32_t tick_ms);

/**
 * @brief Immediately silences the buzzer regardless of the active pattern.
 */
void buzzer_silence(void);

#ifdef __cplusplus
}
#endif

#endif /* BUZZER_H */
