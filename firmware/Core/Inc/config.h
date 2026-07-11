/**
 ******************************************************************************
 * @file    config.h
 * @brief   Global project configuration constants
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 * @attention
 * Original, independent educational firmware. Inspired by the general
 * *behavior* of modern premium-vehicle parking assistance systems but
 * contains no proprietary code, assets, trademarks, or documentation
 * belonging to any vehicle manufacturer.
 ******************************************************************************
 */

#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==========================================================================
 *  SYSTEM CONFIGURATION
 * ==========================================================================*/
#define SYSTEM_TICK_MS                  10U     /* Base scheduler tick (ms) */
#define SELF_TEST_TIMEOUT_MS            2000U   /* Max time allowed for POST */
#define WATCHDOG_TIMEOUT_MS             500U    /* IWDG reload window */

/* ==========================================================================
 *  ULTRASONIC SENSOR CONFIGURATION (HC-SR04)
 * ==========================================================================*/
#define NUM_ULTRASONIC_SENSORS          4U

#define SENSOR_MIN_VALID_DISTANCE_CM    2.0f
#define SENSOR_MAX_VALID_DISTANCE_CM    400.0f

#define SPEED_OF_SOUND_CM_PER_US        0.0343f /* At ~20C, dry air */
#define ECHO_TIMEOUT_US                 30000U  /* ~5m round trip timeout */

#define SENSOR_SCAN_PERIOD_MS           60U     /* Time between full sweeps */
#define SENSOR_SETTLE_TIME_US           10U     /* Trigger pulse width */

/* ==========================================================================
 *  MOVING AVERAGE / MEDIAN FILTER CONFIGURATION
 * ==========================================================================*/
#define FILTER_WINDOW_SIZE              5U      /* Samples per filter window */
#define FILTER_OUTLIER_REJECT_CM        50.0f   /* Max delta between samples */

/* ==========================================================================
 *  PARKING WARNING DISTANCE THRESHOLDS (centimeters)
 * ==========================================================================*/
#define THRESHOLD_SAFE_CM               100.0f
#define THRESHOLD_CAUTION_CM            60.0f
#define THRESHOLD_WARNING_CM            30.0f
#define THRESHOLD_CRITICAL_CM           10.0f
/* Below THRESHOLD_CRITICAL_CM -> EMERGENCY (continuous buzzer + STOP) */

/* ==========================================================================
 *  BUZZER TIMING CONFIGURATION (milliseconds)
 * ==========================================================================*/
#define BUZZER_PERIOD_SLOW_MS           700U
#define BUZZER_PERIOD_MEDIUM_MS         350U
#define BUZZER_PERIOD_FAST_MS           150U
#define BUZZER_PULSE_ON_MS              60U

/* ==========================================================================
 *  SERVO SCANNING CONFIGURATION (SG90)
 * ==========================================================================*/
#define SERVO_MIN_PULSE_US              500U    /* 0 degrees   */
#define SERVO_MAX_PULSE_US              2500U   /* 180 degrees */
#define SERVO_PWM_PERIOD_US             20000U  /* 50 Hz       */

#define SERVO_STEP_COUNT                5U      /* 0,45,90,135,180 */
#define SERVO_DWELL_TIME_MS             150U    /* Settle time before read */

/* ==========================================================================
 *  LCD CONFIGURATION (16x2, PCF8574 I2C backpack)
 * ==========================================================================*/
#define LCD_I2C_ADDRESS                 0x27U   /* 7-bit address, <<1 for HAL */
#define LCD_COLUMNS                     16U
#define LCD_ROWS                        2U
#define LCD_UPDATE_PERIOD_MS            200U

/* ==========================================================================
 *  UART DEBUG CONFIGURATION
 * ==========================================================================*/
#define UART_DEBUG_BAUDRATE             115200U
#define UART_TX_BUFFER_SIZE             256U
#define UART_UPDATE_PERIOD_MS           250U

/* ==========================================================================
 *  BUILD METADATA
 * ==========================================================================*/
#define FW_VERSION_MAJOR                1U
#define FW_VERSION_MINOR                0U
#define FW_VERSION_PATCH                0U
#define FW_PROJECT_NAME                 "Smart-Parking-Assistance-System"

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
