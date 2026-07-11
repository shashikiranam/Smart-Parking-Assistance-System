/**
 ******************************************************************************
 * @file    uart_debug.c
 * @brief   UART debug telemetry output implementation
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#include "uart_debug.h"
#include "config.h"
#include "utilities.h"
#include <stdio.h>
#include <string.h>

static UART_HandleTypeDef *s_huart = NULL;
static char                  s_tx_buffer[UART_TX_BUFFER_SIZE];

void uart_debug_init(UART_HandleTypeDef *huart)
{
    s_huart = huart;

    uart_debug_print("========================================");
    uart_debug_print(FW_PROJECT_NAME " - Debug UART Online");

    snprintf(s_tx_buffer, sizeof(s_tx_buffer), "Firmware v%u.%u.%u",
             (unsigned)FW_VERSION_MAJOR, (unsigned)FW_VERSION_MINOR, (unsigned)FW_VERSION_PATCH);
    uart_debug_print(s_tx_buffer);
    uart_debug_print("========================================");
}

void uart_debug_print(const char *line)
{
    if (s_huart == NULL)
    {
        return;
    }

    HAL_UART_Transmit(s_huart, (uint8_t *)line, (uint16_t)strlen(line), 100U);
    HAL_UART_Transmit(s_huart, (uint8_t *)"\r\n", 2U, 20U);
}

static void append_distance(char *dst, size_t dst_size, const char *label, float value_cm)
{
    int32_t int_part;
    uint32_t dec_part;
    util_split_decimal(value_cm, &int_part, &dec_part);

    char segment[24];
    if (value_cm < 0.0f)
    {
        snprintf(segment, sizeof(segment), "%s:--- ", label);
    }
    else
    {
        snprintf(segment, sizeof(segment), "%s:%ld.%lucm ", label, (long)int_part, (unsigned long)dec_part);
    }
    strncat(dst, segment, dst_size - strlen(dst) - 1U);
}

void uart_debug_report(const decision_input_t *input,
                        const decision_output_t *output,
                        fsm_state_t fsm_state)
{
    if (s_huart == NULL || input == NULL || output == NULL)
    {
        return;
    }

    s_tx_buffer[0] = '\0';

    append_distance(s_tx_buffer, sizeof(s_tx_buffer), "FL", input->front_left_cm);
    append_distance(s_tx_buffer, sizeof(s_tx_buffer), "FR", input->front_right_cm);
    append_distance(s_tx_buffer, sizeof(s_tx_buffer), "RL", input->rear_left_cm);
    append_distance(s_tx_buffer, sizeof(s_tx_buffer), "RR", input->rear_right_cm);

    char tail[96];
    snprintf(tail, sizeof(tail), "STATE:%s SEVERITY:%s ACTION:%s",
             system_state_name(fsm_state),
             decision_severity_name(output->severity),
             decision_action_name(output->action));

    strncat(s_tx_buffer, tail, sizeof(s_tx_buffer) - strlen(s_tx_buffer) - 1U);

    uart_debug_print(s_tx_buffer);
}
