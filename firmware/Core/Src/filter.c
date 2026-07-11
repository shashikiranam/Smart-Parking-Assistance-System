/**
 ******************************************************************************
 * @file    filter.c
 * @brief   Median + EMA filtering implementation for ultrasonic readings
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#include "filter.h"
#include <math.h>

/* Weight applied to a new sample in the exponential moving average.
 * Lower = smoother but slower to react; higher = more responsive. */
#define FILTER_EMA_ALPHA           0.35f
#define FILTER_EMA_ALPHA_OUTLIER   0.10f  /* trust outliers less */

void filter_init(filter_state_t *filt)
{
    for (uint8_t i = 0; i < FILTER_WINDOW_SIZE; i++)
    {
        filt->window[i] = SENSOR_MAX_VALID_DISTANCE_CM;
    }
    filt->write_index    = 0U;
    filt->sample_count   = 0U;
    filt->ema_value      = SENSOR_MAX_VALID_DISTANCE_CM;
    filt->ema_initialized = false;
}

float filter_get_median(const filter_state_t *filt)
{
    float sorted[FILTER_WINDOW_SIZE];
    uint8_t n = filt->sample_count;

    if (n == 0U)
    {
        return SENSOR_MAX_VALID_DISTANCE_CM;
    }

    for (uint8_t i = 0; i < n; i++)
    {
        sorted[i] = filt->window[i];
    }

    /* Small fixed-size insertion sort; n <= FILTER_WINDOW_SIZE (typically 5) */
    for (uint8_t i = 1; i < n; i++)
    {
        float key = sorted[i];
        int8_t j = (int8_t)i - 1;
        while (j >= 0 && sorted[j] > key)
        {
            sorted[j + 1] = sorted[j];
            j--;
        }
        sorted[j + 1] = key;
    }

    if (n % 2U == 1U)
    {
        return sorted[n / 2U];
    }
    else
    {
        return (sorted[n / 2U - 1U] + sorted[n / 2U]) / 2.0f;
    }
}

float filter_process(filter_state_t *filt, float raw_sample)
{
    /* Treat sensor timeout (negative) as "clamp to max range" so a missing
     * echo doesn't get interpreted as an obstacle right on top of the car. */
    float sample = (raw_sample < 0.0f) ? SENSOR_MAX_VALID_DISTANCE_CM : raw_sample;
    sample = fminf(fmaxf(sample, SENSOR_MIN_VALID_DISTANCE_CM), SENSOR_MAX_VALID_DISTANCE_CM);

    filt->window[filt->write_index] = sample;
    filt->write_index = (uint8_t)((filt->write_index + 1U) % FILTER_WINDOW_SIZE);
    if (filt->sample_count < FILTER_WINDOW_SIZE)
    {
        filt->sample_count++;
    }

    float median = filter_get_median(filt);

    if (!filt->ema_initialized)
    {
        filt->ema_value = median;
        filt->ema_initialized = true;
        return filt->ema_value;
    }

    float deviation = fabsf(median - filt->ema_value);
    float alpha = (deviation > FILTER_OUTLIER_REJECT_CM)
                      ? FILTER_EMA_ALPHA_OUTLIER
                      : FILTER_EMA_ALPHA;

    filt->ema_value = (alpha * median) + ((1.0f - alpha) * filt->ema_value);

    return filt->ema_value;
}
