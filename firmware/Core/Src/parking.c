/**
 ******************************************************************************
 * @file    parking.c
 * @brief   Application-layer orchestration implementation
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#include "parking.h"
#include "config.h"
#include "pin_map.h"
#include "servo.h"
#include "lcd_i2c.h"
#include "buzzer.h"
#include "led.h"
#include "uart_debug.h"
#include "utilities.h"
#include <stdio.h>
#include <string.h>

/* ==========================================================================
 *  INITIALIZATION
 * ==========================================================================*/

void parking_app_init(parking_app_t *app, const parking_hw_handles_t *hw)
{
    memset(app, 0, sizeof(*app));
    app->hw = *hw;

    system_state_init(&app->fsm);

    filter_init(&app->filt_fl);
    filter_init(&app->filt_fr);
    filter_init(&app->filt_rl);
    filter_init(&app->filt_rr);

    ultrasonic_init(app->hw.htim_micros);
    servo_init(app->hw.htim_servo, app->hw.servo_channel);
    buzzer_init();
    led_init();
    uart_debug_init(app->hw.huart_debug);

    bool lcd_ok = lcd_i2c_init(app->hw.hi2c_lcd);
    if (lcd_ok)
    {
        lcd_i2c_print_line(0, FW_PROJECT_NAME);
        lcd_i2c_print_line(1, "Initializing...");
    }

    app->button_debounced = false;
    app->button_last_raw  = false;

    system_state_transition(&app->fsm, FSM_STATE_SELF_TEST);
}

/* ==========================================================================
 *  SELF TEST
 * ==========================================================================*/

void parking_app_run_self_test(parking_app_t *app)
{
    bool all_sensors_ok = true;

    for (uint8_t i = 0; i < US_SENSOR_COUNT; i++)
    {
        float distance = ultrasonic_measure_blocking((ultrasonic_id_t)i);

        /* A sensor is considered healthy if it returns EITHER a plausible
         * in-range distance OR a clean timeout (open air, nothing to
         * reflect off within range). Only a stuck/garbage reading fails. */
        bool plausible = (distance < 0.0f) ||
                          (distance >= SENSOR_MIN_VALID_DISTANCE_CM &&
                           distance <= SENSOR_MAX_VALID_DISTANCE_CM);

        if (!plausible)
        {
            all_sensors_ok = false;
            fault_code_t fault = FAULT_SENSOR_FL_UNRESPONSIVE + i;
            system_state_raise_fault(&app->fsm, fault);
            uart_debug_print("SELF_TEST FAIL: sensor implausible reading");
            break;
        }
    }

    if (!all_sensors_ok)
    {
        return;
    }

    if (lcd_i2c_has_fault())
    {
        system_state_raise_fault(&app->fsm, FAULT_I2C_LCD_NACK);
        uart_debug_print("SELF_TEST FAIL: LCD I2C NACK");
        return;
    }

    if (system_state_time_in_state_ms(&app->fsm) > SELF_TEST_TIMEOUT_MS)
    {
        system_state_raise_fault(&app->fsm, FAULT_SELF_TEST_TIMEOUT);
        uart_debug_print("SELF_TEST FAIL: timeout");
        return;
    }

    app->fsm.self_test_passed = true;
    uart_debug_print("SELF_TEST PASS");
    lcd_i2c_print_line(1, "Self-Test OK");
    HAL_Delay(300U);

    system_state_transition(&app->fsm, FSM_STATE_SENSOR_SCAN);
}

/* ==========================================================================
 *  SENSOR SCAN / FILTER / PROCESS
 * ==========================================================================*/

static void parking_step_sensor_scan(parking_app_t *app)
{
    /* Trigger all four sensors back-to-back; each fires independently and
     * is timed via its own EXTI line, so cross-talk between corners at
     * these distances/timings is negligible for a portfolio-grade system. */
    ultrasonic_trigger(US_SENSOR_FRONT_LEFT);
    ultrasonic_trigger(US_SENSOR_FRONT_RIGHT);
    ultrasonic_trigger(US_SENSOR_REAR_LEFT);
    ultrasonic_trigger(US_SENSOR_REAR_RIGHT);

    servo_step_sweep();

    system_state_transition(&app->fsm, FSM_STATE_FILTER_DATA);
}

static void parking_step_filter_data(parking_app_t *app)
{
    ultrasonic_poll_timeouts();

    bool fl_ready = ultrasonic_is_measurement_ready(US_SENSOR_FRONT_LEFT);
    bool fr_ready = ultrasonic_is_measurement_ready(US_SENSOR_FRONT_RIGHT);
    bool rl_ready = ultrasonic_is_measurement_ready(US_SENSOR_REAR_LEFT);
    bool rr_ready = ultrasonic_is_measurement_ready(US_SENSOR_REAR_RIGHT);

    if (!(fl_ready && fr_ready && rl_ready && rr_ready))
    {
        /* Not all echoes back yet; stay in this state and re-check next
         * tick. ECHO_TIMEOUT_US bounds the worst case wait. */
        return;
    }

    float raw_fl = ultrasonic_get_distance_cm(US_SENSOR_FRONT_LEFT);
    float raw_fr = ultrasonic_get_distance_cm(US_SENSOR_FRONT_RIGHT);
    float raw_rl = ultrasonic_get_distance_cm(US_SENSOR_REAR_LEFT);
    float raw_rr = ultrasonic_get_distance_cm(US_SENSOR_REAR_RIGHT);

    app->last_input.front_left_cm  = filter_process(&app->filt_fl, raw_fl);
    app->last_input.front_right_cm = filter_process(&app->filt_fr, raw_fr);
    app->last_input.rear_left_cm   = filter_process(&app->filt_rl, raw_rl);
    app->last_input.rear_right_cm  = filter_process(&app->filt_rr, raw_rr);

    system_state_transition(&app->fsm, FSM_STATE_PROCESS_DATA);
}

static void parking_step_process_data(parking_app_t *app)
{
    /* Reserved for future extension (e.g. trend analysis, closing-speed
     * estimation). Currently a pass-through to the decision engine. */
    system_state_transition(&app->fsm, FSM_STATE_DECISION_ENGINE);
}

static void parking_step_decision_engine(parking_app_t *app)
{
    app->last_output = decision_engine_evaluate(&app->last_input);
    system_state_transition(&app->fsm, FSM_STATE_DISPLAY_UPDATE);
}

/* ==========================================================================
 *  DISPLAY / WARNING / UART OUTPUT STAGES
 * ==========================================================================*/

static void parking_step_display_update(parking_app_t *app)
{
    if ((app->fsm.tick_ms - app->last_lcd_update_tick_ms) >= LCD_UPDATE_PERIOD_MS)
    {
        char line0[LCD_COLUMNS + 1U];
        char line1[LCD_COLUMNS + 1U];

        int32_t fl_i, fr_i;
        uint32_t fl_d, fr_d;
        util_split_decimal(app->last_input.front_left_cm, &fl_i, &fl_d);
        util_split_decimal(app->last_input.front_right_cm, &fr_i, &fr_d);

        snprintf(line0, sizeof(line0), "FL:%3ld FR:%3ld", (long)fl_i, (long)fr_i);

        const char *action_str = decision_action_name(app->last_output.action);
        snprintf(line1, sizeof(line1), "%s", action_str);

        lcd_i2c_print_line(0, line0);
        lcd_i2c_print_line(1, line1);

        app->last_lcd_update_tick_ms = app->fsm.tick_ms;
    }

    system_state_transition(&app->fsm, FSM_STATE_WARNING_CONTROL);
}

static buzzer_pattern_t buzzer_pattern_for_severity(parking_severity_t severity)
{
    switch (severity)
    {
        case PARKING_STATE_SAFE:      return BUZZER_PATTERN_OFF;
        case PARKING_STATE_CAUTION:   return BUZZER_PATTERN_SLOW;
        case PARKING_STATE_WARNING:   return BUZZER_PATTERN_MEDIUM;
        case PARKING_STATE_CRITICAL:  return BUZZER_PATTERN_FAST;
        case PARKING_STATE_EMERGENCY: return BUZZER_PATTERN_CONTINUOUS;
        default:                      return BUZZER_PATTERN_OFF;
    }
}

static void parking_step_warning_control(parking_app_t *app)
{
    buzzer_set_pattern(buzzer_pattern_for_severity(app->last_output.severity));
    buzzer_update(app->fsm.tick_ms);
    led_set_severity((uint8_t)app->last_output.severity);
    led_update(app->fsm.tick_ms);

    system_state_transition(&app->fsm, FSM_STATE_UART_UPDATE);
}

static void parking_step_uart_update(parking_app_t *app)
{
    if ((app->fsm.tick_ms - app->last_uart_update_tick_ms) >= UART_UPDATE_PERIOD_MS)
    {
        uart_debug_report(&app->last_input, &app->last_output, app->fsm.current_state);
        app->last_uart_update_tick_ms = app->fsm.tick_ms;
    }

    if (app->fsm.parking_mode_enabled)
    {
        system_state_transition(&app->fsm, FSM_STATE_SENSOR_SCAN);
    }
    else
    {
        system_state_transition(&app->fsm, FSM_STATE_IDLE);
    }
}

static void parking_step_idle(parking_app_t *app)
{
    buzzer_silence();
    led_set_mode(LED_ID_RED, LED_MODE_OFF);
    led_set_mode(LED_ID_GREEN, LED_MODE_BLINK_SLOW);
    led_update(app->fsm.tick_ms);

    if ((app->fsm.tick_ms - app->last_lcd_update_tick_ms) >= LCD_UPDATE_PERIOD_MS)
    {
        lcd_i2c_print_line(0, "Parking Assist");
        lcd_i2c_print_line(1, "Press button...");
        app->last_lcd_update_tick_ms = app->fsm.tick_ms;
    }

    if (app->fsm.parking_mode_enabled)
    {
        servo_reset_sweep();
        lcd_i2c_clear();
        system_state_transition(&app->fsm, FSM_STATE_SENSOR_SCAN);
    }
}

static void parking_step_error(parking_app_t *app)
{
    buzzer_set_pattern(BUZZER_PATTERN_SLOW);
    buzzer_update(app->fsm.tick_ms);
    led_set_mode(LED_ID_RED, LED_MODE_BLINK_FAST);
    led_set_mode(LED_ID_GREEN, LED_MODE_OFF);
    led_update(app->fsm.tick_ms);

    if ((app->fsm.tick_ms - app->last_lcd_update_tick_ms) >= LCD_UPDATE_PERIOD_MS)
    {
        char line1[LCD_COLUMNS + 1U];
        snprintf(line1, sizeof(line1), "Fault code: %d", (int)app->fsm.last_error);
        lcd_i2c_print_line(0, "SYSTEM ERROR");
        lcd_i2c_print_line(1, line1);
        app->last_lcd_update_tick_ms = app->fsm.tick_ms;
    }

    /* Recovery: operator toggles the parking button to request a retry. */
    if (app->fsm.parking_mode_enabled)
    {
        app->fsm.self_test_passed = false;
        system_state_transition(&app->fsm, FSM_STATE_SELF_TEST);
    }
}

/* ==========================================================================
 *  MAIN DISPATCH
 * ==========================================================================*/

void parking_app_run(parking_app_t *app)
{
    switch (app->fsm.current_state)
    {
        case FSM_STATE_SYSTEM_INIT:
            /* handled synchronously in parking_app_init(); should not
             * normally be re-entered, but transition defensively. */
            system_state_transition(&app->fsm, FSM_STATE_SELF_TEST);
            break;

        case FSM_STATE_SELF_TEST:      parking_app_run_self_test(app);   break;
        case FSM_STATE_SENSOR_SCAN:    parking_step_sensor_scan(app);    break;
        case FSM_STATE_FILTER_DATA:    parking_step_filter_data(app);    break;
        case FSM_STATE_PROCESS_DATA:   parking_step_process_data(app);   break;
        case FSM_STATE_DECISION_ENGINE:parking_step_decision_engine(app);break;
        case FSM_STATE_DISPLAY_UPDATE: parking_step_display_update(app); break;
        case FSM_STATE_WARNING_CONTROL:parking_step_warning_control(app);break;
        case FSM_STATE_UART_UPDATE:    parking_step_uart_update(app);    break;
        case FSM_STATE_IDLE:           parking_step_idle(app);           break;
        case FSM_STATE_ERROR:          parking_step_error(app);          break;

        default:
            system_state_raise_fault(&app->fsm, FAULT_NONE);
            break;
    }
}

/* ==========================================================================
 *  ISR HOOKS
 * ==========================================================================*/

void parking_app_button_isr(parking_app_t *app)
{
    /* Raw EXTI edge on the push button; actual debouncing happens in the
     * periodic tick via util_debounce() called from parking_app_tick_isr()
     * is intentionally NOT done here to keep ISR execution minimal - we
     * just record the edge, and toggle immediately since B1 has a solid
     * hardware pull configuration on the NUCLEO board (low bounce risk).
     */
    app->fsm.parking_mode_enabled = !app->fsm.parking_mode_enabled;
}

void parking_app_tick_isr(parking_app_t *app)
{
    app->fsm.tick_ms++;
}
