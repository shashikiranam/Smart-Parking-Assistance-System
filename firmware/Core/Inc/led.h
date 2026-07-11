/**
 ******************************************************************************
 * @file    led.h
 * @brief   Status LED driver (Red = danger, Green = safe, heartbeat)
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LED_ID_GREEN = 0,
    LED_ID_RED,
    LED_ID_HEARTBEAT,
    LED_ID_COUNT
} led_id_t;

typedef enum
{
    LED_MODE_OFF = 0,
    LED_MODE_ON,
    LED_MODE_BLINK_SLOW,
    LED_MODE_BLINK_FAST
} led_mode_t;

/**
 * @brief Initialize all status LED GPIOs to the OFF state.
 */
void led_init(void);

/**
 * @brief Set the display mode for a specific LED.
 */
void led_set_mode(led_id_t led, led_mode_t mode);

/**
 * @brief Must be called periodically (every scheduler tick) to service any
 *        LEDs currently in a blink mode.
 *
 * @param tick_ms Current monotonic millisecond tick.
 */
void led_update(uint32_t tick_ms);

/**
 * @brief Convenience helper mapping overall parking severity directly to
 *        the red/green LED pair (green=safe/caution, red=warning/critical).
 *
 * @param severity_level 0=Safe .. 4=Emergency (see parking_state_t)
 */
void led_set_severity(uint8_t severity_level);

#ifdef __cplusplus
}
#endif

#endif /* LED_H */
