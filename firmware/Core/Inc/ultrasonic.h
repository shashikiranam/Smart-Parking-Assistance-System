/**
 ******************************************************************************
 * @file    ultrasonic.h
 * @brief   HC-SR04 ultrasonic sensor driver (trigger + EXTI echo timing)
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32g0xx_hal.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    US_SENSOR_FRONT_LEFT = 0,
    US_SENSOR_FRONT_RIGHT,
    US_SENSOR_REAR_LEFT,
    US_SENSOR_REAR_RIGHT,
    US_SENSOR_COUNT
} ultrasonic_id_t;

typedef struct
{
    GPIO_TypeDef *trig_port;
    uint16_t      trig_pin;
    GPIO_TypeDef *echo_port;
    uint16_t      echo_pin;
} ultrasonic_pin_config_t;

typedef struct
{
    volatile uint32_t rise_timestamp_us;
    volatile uint32_t fall_timestamp_us;
    volatile bool     echo_captured;
    volatile bool     echo_pending;
    float              last_distance_cm;   /* -1.0f = timeout / no echo */
} ultrasonic_channel_t;

/**
 * @brief Initialize the ultrasonic driver: configures GPIOs and internal
 *        per-channel state. Must be called once after HAL_Init and after
 *        the microsecond free-running timer (TIM2) has been started.
 *
 * @param htim Handle to the free-running microsecond timer (see pin_map.h,
 *             MICROS_TIMER_INSTANCE), used to timestamp echo edges.
 */
void ultrasonic_init(TIM_HandleTypeDef *htim);

/**
 * @brief Fire a 10us trigger pulse on the given sensor and start listening
 *        for its echo. Non-blocking: result is retrieved later via
 *        ultrasonic_get_distance_cm() once ultrasonic_is_measurement_ready()
 *        returns true, or after ECHO_TIMEOUT_US has elapsed.
 */
void ultrasonic_trigger(ultrasonic_id_t sensor);

/**
 * @brief Must be called periodically (every scheduler tick) to detect and
 *        finalize timed-out measurements (no echo received).
 */
void ultrasonic_poll_timeouts(void);

/**
 * @brief Returns true once a triggered measurement has completed (either a
 *        valid echo was captured or it timed out).
 */
bool ultrasonic_is_measurement_ready(ultrasonic_id_t sensor);

/**
 * @brief Returns the last computed distance in centimeters for the given
 *        sensor. Returns -1.0f if the last measurement timed out.
 */
float ultrasonic_get_distance_cm(ultrasonic_id_t sensor);

/**
 * @brief Blocking convenience wrapper: triggers the sensor and busy-waits
 *        (bounded by ECHO_TIMEOUT_US) for the result. Used only during
 *        SELF_TEST where a simple synchronous check is acceptable.
 */
float ultrasonic_measure_blocking(ultrasonic_id_t sensor);

/**
 * @brief EXTI callback hook. Must be called from HAL_GPIO_EXTI_Rising_Callback
 *        / HAL_GPIO_EXTI_Falling_Callback (or a combined edge callback) in
 *        main.c / stm32g0xx_it.c for each of the four echo pins.
 */
void ultrasonic_exti_handler(ultrasonic_id_t sensor, bool rising_edge);

/**
 * @brief Returns the microsecond free-running counter value at the moment
 *        of call. Backed by MICROS_TIMER_INSTANCE (see pin_map.h).
 */
uint32_t ultrasonic_get_micros(void);

const char *ultrasonic_sensor_name(ultrasonic_id_t sensor);

#ifdef __cplusplus
}
#endif

#endif /* ULTRASONIC_H */
