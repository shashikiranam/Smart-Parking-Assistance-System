/**
 ******************************************************************************
 * @file    servo.h
 * @brief   SG90 servo driver for ultrasonic sweep scanning
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32g0xx_hal.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the servo driver on the given PWM timer channel and
 *        move it to the 90 degree (center) position.
 *
 * @param htim    Timer handle configured for PWM (see cube_mx/*.ioc, TIM3).
 * @param channel Timer channel, e.g. TIM_CHANNEL_1.
 */
void servo_init(TIM_HandleTypeDef *htim, uint32_t channel);

/**
 * @brief Command the servo to a specific angle.
 * @param angle_deg Angle in degrees, clamped internally to [0, 180].
 */
void servo_set_angle(uint16_t angle_deg);

/**
 * @brief Returns the last commanded angle in degrees.
 */
uint16_t servo_get_angle(void);

/**
 * @brief Advances the sweep sequence (0,45,90,135,180,...) by one step and
 *        commands the servo to the new angle. Intended to be called once
 *        per SENSOR_SCAN state entry.
 *
 * @return The new angle in degrees.
 */
uint16_t servo_step_sweep(void);

/**
 * @brief Resets the sweep sequence back to its first step (0 degrees).
 */
void servo_reset_sweep(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVO_H */
