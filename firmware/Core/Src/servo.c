/**
 ******************************************************************************
 * @file    servo.c
 * @brief   SG90 servo driver implementation (hardware PWM)
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#include "servo.h"
#include "utilities.h"

static TIM_HandleTypeDef *s_htim   = NULL;
static uint32_t            s_channel = 0U;
static uint16_t             s_current_angle = 90U;
static uint8_t              s_sweep_index   = 0U;

/* Sweep sequence as specified: 0, 45, 90, 135, 180, then back to 0 */
static const uint16_t s_sweep_angles[SERVO_STEP_COUNT] = { 0U, 45U, 90U, 135U, 180U };

static uint32_t servo_angle_to_pulse_us(uint16_t angle_deg)
{
    float pulse = util_map_float((float)angle_deg, 0.0f, 180.0f,
                                  (float)SERVO_MIN_PULSE_US, (float)SERVO_MAX_PULSE_US);
    return (uint32_t)pulse;
}

void servo_init(TIM_HandleTypeDef *htim, uint32_t channel)
{
    s_htim    = htim;
    s_channel = channel;
    s_sweep_index = 0U;

    HAL_TIM_PWM_Start(s_htim, s_channel);
    servo_set_angle(90U); /* start centered */
}

void servo_set_angle(uint16_t angle_deg)
{
    if (s_htim == NULL)
    {
        return;
    }

    uint16_t clamped = (uint16_t)util_clamp_u32(angle_deg, 0U, 180U);
    uint32_t pulse_us = servo_angle_to_pulse_us(clamped);

    /* ARR is configured (via CubeMX) so that each timer tick == 1us and the
     * PWM period equals SERVO_PWM_PERIOD_US (20000 -> 50Hz). CCR is simply
     * the desired high-time in microseconds. */
    __HAL_TIM_SET_COMPARE(s_htim, s_channel, pulse_us);

    s_current_angle = clamped;
}

uint16_t servo_get_angle(void)
{
    return s_current_angle;
}

uint16_t servo_step_sweep(void)
{
    uint16_t angle = s_sweep_angles[s_sweep_index];
    servo_set_angle(angle);

    s_sweep_index = (uint8_t)((s_sweep_index + 1U) % SERVO_STEP_COUNT);

    return angle;
}

void servo_reset_sweep(void)
{
    s_sweep_index = 0U;
    servo_set_angle(s_sweep_angles[0]);
}
