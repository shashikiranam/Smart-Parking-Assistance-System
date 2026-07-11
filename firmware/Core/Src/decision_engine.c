/**
 ******************************************************************************
 * @file    decision_engine.c
 * @brief   Parking decision logic implementation
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 * Decision priority (highest first) - safety-critical stop conditions are
 * always evaluated before cosmetic steering guidance:
 *
 *   1. STOP       - front or rear corner has reached CRITICAL/EMERGENCY range
 *   2. TURN_RIGHT - left side is meaningfully closer to an obstacle than right
 *   3. TURN_LEFT  - right side is meaningfully closer to an obstacle than left
 *   4. CENTERED   - left/right sides are balanced but not fully clear
 *   5. PROCEED_SLOWLY - all four corners report SAFE range
 ******************************************************************************
 */

#include "decision_engine.h"
#include "config.h"
#include "utilities.h"

/* Minimum left/right delta (cm) before we bother suggesting a turn; avoids
 * flickering "Turn Left"/"Turn Right" advice from sensor noise when the
 * vehicle is essentially centered. */
#define STEERING_DEADBAND_CM   8.0f

parking_severity_t decision_classify_distance(float distance_cm)
{
    if (distance_cm < 0.0f || distance_cm > THRESHOLD_SAFE_CM)
    {
        return PARKING_STATE_SAFE;
    }
    if (distance_cm > THRESHOLD_CAUTION_CM)
    {
        return PARKING_STATE_CAUTION;
    }
    if (distance_cm > THRESHOLD_WARNING_CM)
    {
        return PARKING_STATE_WARNING;
    }
    if (distance_cm > THRESHOLD_CRITICAL_CM)
    {
        return PARKING_STATE_CRITICAL;
    }
    return PARKING_STATE_EMERGENCY;
}

static parking_severity_t severity_max(parking_severity_t a, parking_severity_t b)
{
    return (a > b) ? a : b;
}

decision_output_t decision_engine_evaluate(const decision_input_t *input)
{
    decision_output_t out;

    parking_severity_t sev_fl = decision_classify_distance(input->front_left_cm);
    parking_severity_t sev_fr = decision_classify_distance(input->front_right_cm);
    parking_severity_t sev_rl = decision_classify_distance(input->rear_left_cm);
    parking_severity_t sev_rr = decision_classify_distance(input->rear_right_cm);

    out.severity = severity_max(severity_max(sev_fl, sev_fr), severity_max(sev_rl, sev_rr));

    out.front_critical = (sev_fl >= PARKING_STATE_CRITICAL) || (sev_fr >= PARKING_STATE_CRITICAL);
    out.rear_critical  = (sev_rl >= PARKING_STATE_CRITICAL) || (sev_rr >= PARKING_STATE_CRITICAL);

    out.closest_distance_cm = util_min_f(util_min_f(input->front_left_cm, input->front_right_cm),
                                          util_min_f(input->rear_left_cm, input->rear_right_cm));

    float left_min  = util_min_f(input->front_left_cm, input->rear_left_cm);
    float right_min = util_min_f(input->front_right_cm, input->rear_right_cm);
    float lr_delta   = left_min - right_min;

    if (out.front_critical || out.rear_critical)
    {
        out.action = ACTION_STOP;
    }
    else if (lr_delta < -STEERING_DEADBAND_CM)
    {
        /* left side closer than right -> steer away from the left obstacle */
        out.action = ACTION_TURN_RIGHT;
    }
    else if (lr_delta > STEERING_DEADBAND_CM)
    {
        /* right side closer than left -> steer away from the right obstacle */
        out.action = ACTION_TURN_LEFT;
    }
    else if (out.severity == PARKING_STATE_SAFE)
    {
        out.action = ACTION_PROCEED_SLOWLY;
    }
    else
    {
        out.action = ACTION_CENTERED;
    }

    return out;
}

const char *decision_severity_name(parking_severity_t severity)
{
    switch (severity)
    {
        case PARKING_STATE_SAFE:      return "SAFE";
        case PARKING_STATE_CAUTION:   return "CAUTION";
        case PARKING_STATE_WARNING:   return "WARNING";
        case PARKING_STATE_CRITICAL:  return "CRITICAL";
        case PARKING_STATE_EMERGENCY: return "EMERGENCY";
        default:                      return "UNKNOWN";
    }
}

const char *decision_action_name(parking_action_t action)
{
    switch (action)
    {
        case ACTION_PROCEED_SLOWLY: return "Proceed Slowly";
        case ACTION_CENTERED:       return "Centered";
        case ACTION_TURN_LEFT:      return "Turn Left";
        case ACTION_TURN_RIGHT:     return "Turn Right";
        case ACTION_STOP:           return "STOP";
        default:                    return "Unknown";
    }
}
