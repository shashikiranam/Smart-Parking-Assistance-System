/**
 ******************************************************************************
 * @file    ultrasonic.c
 * @brief   HC-SR04 ultrasonic sensor driver implementation
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 * Measurement strategy:
 *   1. ultrasonic_trigger() drives TRIG high for >=10us then low.
 *   2. The sensor's ECHO pin is configured as an EXTI input (both edges).
 *   3. On rising edge, ultrasonic_exti_handler() latches the current
 *      microsecond timestamp (from the free-running TIM2 counter).
 *   4. On falling edge, the pulse width (fall - rise) is converted to a
 *      distance using the speed of sound and stored as the channel result.
 *   5. ultrasonic_poll_timeouts() finalizes any channel that never saw a
 *      falling edge within ECHO_TIMEOUT_US (no obstacle in range / no echo).
 ******************************************************************************
 */

#include "ultrasonic.h"
#include "pin_map.h"
#include <string.h>

static TIM_HandleTypeDef      *s_htim_micros = NULL;
static ultrasonic_channel_t    s_channels[US_SENSOR_COUNT];
static uint32_t                s_trigger_tick_us[US_SENSOR_COUNT];

static const ultrasonic_pin_config_t s_pin_config[US_SENSOR_COUNT] =
{
    [US_SENSOR_FRONT_LEFT]  = { US_FL_TRIG_PORT, US_FL_TRIG_PIN, US_FL_ECHO_PORT, US_FL_ECHO_PIN },
    [US_SENSOR_FRONT_RIGHT] = { US_FR_TRIG_PORT, US_FR_TRIG_PIN, US_FR_ECHO_PORT, US_FR_ECHO_PIN },
    [US_SENSOR_REAR_LEFT]   = { US_RL_TRIG_PORT, US_RL_TRIG_PIN, US_RL_ECHO_PORT, US_RL_ECHO_PIN },
    [US_SENSOR_REAR_RIGHT]  = { US_RR_TRIG_PORT, US_RR_TRIG_PIN, US_RR_ECHO_PORT, US_RR_ECHO_PIN },
};

static void ultrasonic_delay_us(uint32_t us)
{
    /* Busy-wait using the free-running microsecond timer for accurate,
     * short (<20us) delays without touching the RTOS/scheduler tick. */
    uint32_t start = ultrasonic_get_micros();
    while ((ultrasonic_get_micros() - start) < us)
    {
        /* spin */
    }
}

void ultrasonic_init(TIM_HandleTypeDef *htim)
{
    s_htim_micros = htim;
    memset(s_channels, 0, sizeof(s_channels));
    memset(s_trigger_tick_us, 0, sizeof(s_trigger_tick_us));

    for (uint8_t i = 0; i < US_SENSOR_COUNT; i++)
    {
        s_channels[i].last_distance_cm = -1.0f;
        HAL_GPIO_WritePin(s_pin_config[i].trig_port, s_pin_config[i].trig_pin, GPIO_PIN_RESET);
    }
}

uint32_t ultrasonic_get_micros(void)
{
    if (s_htim_micros == NULL)
    {
        return 0U;
    }
    return __HAL_TIM_GET_COUNTER(s_htim_micros);
}

void ultrasonic_trigger(ultrasonic_id_t sensor)
{
    if (sensor >= US_SENSOR_COUNT)
    {
        return;
    }

    ultrasonic_channel_t *ch = &s_channels[sensor];
    ch->echo_captured = false;
    ch->echo_pending   = true;
    ch->rise_timestamp_us = 0U;
    ch->fall_timestamp_us = 0U;

    const ultrasonic_pin_config_t *pins = &s_pin_config[sensor];

    HAL_GPIO_WritePin(pins->trig_port, pins->trig_pin, GPIO_PIN_SET);
    ultrasonic_delay_us(SENSOR_SETTLE_TIME_US);
    HAL_GPIO_WritePin(pins->trig_port, pins->trig_pin, GPIO_PIN_RESET);

    s_trigger_tick_us[sensor] = ultrasonic_get_micros();
}

void ultrasonic_exti_handler(ultrasonic_id_t sensor, bool rising_edge)
{
    if (sensor >= US_SENSOR_COUNT)
    {
        return;
    }

    ultrasonic_channel_t *ch = &s_channels[sensor];
    uint32_t now = ultrasonic_get_micros();

    if (rising_edge)
    {
        ch->rise_timestamp_us = now;
    }
    else
    {
        if (ch->echo_pending)
        {
            ch->fall_timestamp_us = now;

            uint32_t pulse_width_us = ch->fall_timestamp_us - ch->rise_timestamp_us;
            float distance_cm = ((float)pulse_width_us * SPEED_OF_SOUND_CM_PER_US) / 2.0f;

            if (distance_cm < SENSOR_MIN_VALID_DISTANCE_CM ||
                distance_cm > SENSOR_MAX_VALID_DISTANCE_CM)
            {
                ch->last_distance_cm = -1.0f; /* out of rated sensor range */
            }
            else
            {
                ch->last_distance_cm = distance_cm;
            }

            ch->echo_captured = true;
            ch->echo_pending   = false;
        }
    }
}

void ultrasonic_poll_timeouts(void)
{
    uint32_t now = ultrasonic_get_micros();

    for (uint8_t i = 0; i < US_SENSOR_COUNT; i++)
    {
        ultrasonic_channel_t *ch = &s_channels[i];

        if (ch->echo_pending)
        {
            uint32_t elapsed = now - s_trigger_tick_us[i];
            if (elapsed >= ECHO_TIMEOUT_US)
            {
                ch->last_distance_cm = -1.0f; /* no echo -> treat as clear */
                ch->echo_captured    = true;
                ch->echo_pending     = false;
            }
        }
    }
}

bool ultrasonic_is_measurement_ready(ultrasonic_id_t sensor)
{
    if (sensor >= US_SENSOR_COUNT)
    {
        return false;
    }
    return s_channels[sensor].echo_captured;
}

float ultrasonic_get_distance_cm(ultrasonic_id_t sensor)
{
    if (sensor >= US_SENSOR_COUNT)
    {
        return -1.0f;
    }
    return s_channels[sensor].last_distance_cm;
}

float ultrasonic_measure_blocking(ultrasonic_id_t sensor)
{
    if (sensor >= US_SENSOR_COUNT)
    {
        return -1.0f;
    }

    ultrasonic_trigger(sensor);

    uint32_t start = ultrasonic_get_micros();
    while (!ultrasonic_is_measurement_ready(sensor))
    {
        ultrasonic_poll_timeouts();
        if ((ultrasonic_get_micros() - start) > (ECHO_TIMEOUT_US * 2U))
        {
            break; /* hard safety bound in case of stuck EXTI */
        }
    }

    return ultrasonic_get_distance_cm(sensor);
}

const char *ultrasonic_sensor_name(ultrasonic_id_t sensor)
{
    switch (sensor)
    {
        case US_SENSOR_FRONT_LEFT:  return "FL";
        case US_SENSOR_FRONT_RIGHT: return "FR";
        case US_SENSOR_REAR_LEFT:   return "RL";
        case US_SENSOR_REAR_RIGHT:  return "RR";
        default:                    return "??";
    }
}
