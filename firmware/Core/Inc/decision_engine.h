/**
 ******************************************************************************
 * @file    decision_engine.h
 * @brief   Parking decision logic: severity classification + steering advice
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#ifndef DECISION_ENGINE_H
#define DECISION_ENGINE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Overall parking severity, ordered from safest to most dangerous.
 *        Numeric value doubles as the LED/buzzer severity index.
 */
typedef enum
{
    PARKING_STATE_SAFE = 0,
    PARKING_STATE_CAUTION,
    PARKING_STATE_WARNING,
    PARKING_STATE_CRITICAL,
    PARKING_STATE_EMERGENCY
} parking_severity_t;

/**
 * @brief Driver guidance action, displayed on the LCD "Action:" line.
 */
typedef enum
{
    ACTION_PROCEED_SLOWLY = 0,
    ACTION_CENTERED,
    ACTION_TURN_LEFT,
    ACTION_TURN_RIGHT,
    ACTION_STOP
} parking_action_t;

/**
 * @brief Snapshot of the four filtered sensor distances (centimeters),
 *        as produced by the sensor/filter layer and consumed by the
 *        decision engine.
 */
typedef struct
{
    float front_left_cm;
    float front_right_cm;
    float rear_left_cm;
    float rear_right_cm;
} decision_input_t;

/**
 * @brief Result of the decision engine evaluation for a single cycle.
 */
typedef struct
{
    parking_severity_t severity;
    parking_action_t   action;
    float               closest_distance_cm;
    bool                front_critical;
    bool                rear_critical;
} decision_output_t;

/**
 * @brief Classify a single distance reading into a severity level using
 *        the THRESHOLD_* constants from config.h.
 */
parking_severity_t decision_classify_distance(float distance_cm);

/**
 * @brief Run the full decision engine for one cycle: determines overall
 *        severity (worst of the four corners), whether to stop (front/rear
 *        critical), and steering guidance (compares left vs right sides).
 */
decision_output_t decision_engine_evaluate(const decision_input_t *input);

const char *decision_severity_name(parking_severity_t severity);
const char *decision_action_name(parking_action_t action);

#ifdef __cplusplus
}
#endif

#endif /* DECISION_ENGINE_H */
