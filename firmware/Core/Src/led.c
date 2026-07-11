/**
 ******************************************************************************
 * @file    led.c
 * @brief   Status LED driver implementation
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#include "led.h"
#include "pin_map.h"
#include "stm32g0xx_hal.h"

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t      pin;
    led_mode_t    mode;
    uint32_t      last_toggle_ms;
    bool          state_on;
} led_channel_t;

#define LED_BLINK_SLOW_PERIOD_MS   500U
#define LED_BLINK_FAST_PERIOD_MS   150U

static led_channel_t s_leds[LED_ID_COUNT] =
{
    [LED_ID_GREEN]     = { LED_GREEN_PORT,     LED_GREEN_PIN,     LED_MODE_OFF, 0U, false },
    [LED_ID_RED]       = { LED_RED_PORT,       LED_RED_PIN,       LED_MODE_OFF, 0U, false },
    [LED_ID_HEARTBEAT] = { LED_HEARTBEAT_PORT, LED_HEARTBEAT_PIN, LED_MODE_OFF, 0U, false },
};

static void led_write(led_channel_t *ch, bool on)
{
    ch->state_on = on;
    HAL_GPIO_WritePin(ch->port, ch->pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void led_init(void)
{
    for (uint8_t i = 0; i < LED_ID_COUNT; i++)
    {
        s_leds[i].mode = LED_MODE_OFF;
        led_write(&s_leds[i], false);
    }
}

void led_set_mode(led_id_t led, led_mode_t mode)
{
    if (led >= LED_ID_COUNT)
    {
        return;
    }

    if (s_leds[led].mode != mode)
    {
        s_leds[led].mode = mode;
        s_leds[led].last_toggle_ms = 0U;
    }
}

void led_update(uint32_t tick_ms)
{
    for (uint8_t i = 0; i < LED_ID_COUNT; i++)
    {
        led_channel_t *ch = &s_leds[i];

        switch (ch->mode)
        {
            case LED_MODE_OFF:
                led_write(ch, false);
                break;

            case LED_MODE_ON:
                led_write(ch, true);
                break;

            case LED_MODE_BLINK_SLOW:
            case LED_MODE_BLINK_FAST:
            {
                uint32_t period = (ch->mode == LED_MODE_BLINK_SLOW)
                                       ? LED_BLINK_SLOW_PERIOD_MS
                                       : LED_BLINK_FAST_PERIOD_MS;

                if ((tick_ms - ch->last_toggle_ms) >= (period / 2U))
                {
                    led_write(ch, !ch->state_on);
                    ch->last_toggle_ms = tick_ms;
                }
                break;
            }

            default:
                break;
        }
    }
}

void led_set_severity(uint8_t severity_level)
{
    /* 0=Safe, 1=Caution, 2=Warning, 3=Critical, 4=Emergency */
    switch (severity_level)
    {
        case 0U: /* Safe */
            led_set_mode(LED_ID_GREEN, LED_MODE_ON);
            led_set_mode(LED_ID_RED, LED_MODE_OFF);
            break;

        case 1U: /* Caution */
            led_set_mode(LED_ID_GREEN, LED_MODE_BLINK_SLOW);
            led_set_mode(LED_ID_RED, LED_MODE_OFF);
            break;

        case 2U: /* Warning */
            led_set_mode(LED_ID_GREEN, LED_MODE_OFF);
            led_set_mode(LED_ID_RED, LED_MODE_BLINK_SLOW);
            break;

        case 3U: /* Critical */
            led_set_mode(LED_ID_GREEN, LED_MODE_OFF);
            led_set_mode(LED_ID_RED, LED_MODE_BLINK_FAST);
            break;

        case 4U: /* Emergency */
        default:
            led_set_mode(LED_ID_GREEN, LED_MODE_OFF);
            led_set_mode(LED_ID_RED, LED_MODE_ON);
            break;
    }
}
