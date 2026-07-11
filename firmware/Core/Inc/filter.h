/**
 ******************************************************************************
 * @file    filter.h
 * @brief   Noise filtering for ultrasonic distance readings
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 * Implements a rolling median filter (robust to single-sample HC-SR04
 * dropouts/echoes) followed by an exponential moving average (smooths the
 * displayed value without adding excessive latency).
 ******************************************************************************
 */

#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float    window[FILTER_WINDOW_SIZE];
    uint8_t  write_index;
    uint8_t  sample_count;      /* number of valid samples currently held */
    float    ema_value;         /* exponential moving average output      */
    bool     ema_initialized;
} filter_state_t;

/**
 * @brief Reset a filter instance to its initial (empty) state.
 */
void filter_init(filter_state_t *filt);

/**
 * @brief Push a new raw distance sample (cm) through the filter.
 *
 * Samples that deviate from the current median by more than
 * FILTER_OUTLIER_REJECT_CM are treated as outliers: they are still stored
 * (to avoid a "stuck" filter if the vehicle genuinely moved fast) but do
 * not influence the EMA as strongly.
 *
 * @param filt       Filter instance.
 * @param raw_sample Raw distance in centimeters. Pass a negative value to
 *                    indicate "no echo received" (sensor timeout).
 * @return Filtered distance in centimeters.
 */
float filter_process(filter_state_t *filt, float raw_sample);

/**
 * @brief Compute the median of the samples currently held in the window.
 */
float filter_get_median(const filter_state_t *filt);

#ifdef __cplusplus
}
#endif

#endif /* FILTER_H */
