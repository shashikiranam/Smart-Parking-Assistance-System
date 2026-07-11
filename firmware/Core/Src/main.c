/**
 ******************************************************************************
 * @file    main.c
 * @brief   Application entry point: peripheral bring-up + FSM main loop
 * @project Smart-Parking-Assistance-System
 * @board   STM32 NUCLEO-G071RB (STM32G071RBT6)
 ******************************************************************************
 * @attention
 * This file follows the standard STM32CubeIDE/CubeMX project layout so the
 * generated peripheral init functions (SystemClock_Config, MX_*_Init) can be
 * regenerated/extended from the accompanying cube_mx/*.ioc file without
 * losing the hand-written application glue below the "USER CODE" markers.
 ******************************************************************************
 */

#include "main.h"
#include "parking.h"
#include "ultrasonic.h"
#include "uart_debug.h"

/* ==========================================================================
 *  GLOBAL PERIPHERAL HANDLES
 * ==========================================================================*/
I2C_HandleTypeDef  hi2c1;
TIM_HandleTypeDef  htim2;
TIM_HandleTypeDef  htim3;
UART_HandleTypeDef huart2;

/* ==========================================================================
 *  APPLICATION STATE
 * ==========================================================================*/
static parking_app_t s_app;

/* ==========================================================================
 *  PRIVATE FUNCTION PROTOTYPES
 * ==========================================================================*/
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);   /* free-running microsecond base timer */
static void MX_TIM3_Init(void);   /* servo PWM                            */
static void MX_USART2_UART_Init(void);
static void MX_NVIC_Init(void);

/* ==========================================================================
 *  MAIN
 * ==========================================================================*/
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_USART2_UART_Init();
    MX_NVIC_Init();

    HAL_TIM_Base_Start(&htim2); /* start free-running microsecond counter */

    parking_hw_handles_t hw =
    {
        .hi2c_lcd     = &hi2c1,
        .huart_debug  = &huart2,
        .htim_micros  = &htim2,
        .htim_servo   = &htim3,
        .servo_channel = TIM_CHANNEL_1,
    };

    parking_app_init(&s_app, &hw);

    uint32_t last_tick_ms = HAL_GetTick();

    while (1)
    {
        uint32_t now = HAL_GetTick();

        /* Drive the FSM's internal tick counter from HAL_GetTick() rather
         * than a dedicated ISR, since HAL_GetTick() is already updated by
         * the SysTick interrupt at 1ms resolution - this keeps the design
         * simple while still satisfying the "tick_ms advances every 1ms"
         * contract expected by system_state_t. */
        while (last_tick_ms != now)
        {
            parking_app_tick_isr(&s_app);
            last_tick_ms++;
        }

        parking_app_run(&s_app);

        /* SYSTEM_TICK_MS paces the FSM so downstream stages (buzzer/LED
         * pattern generation, LCD refresh) have consistent timing without
         * pegging the CPU at 100%. Only applies while not blocked inside
         * a state waiting on hardware (e.g. FILTER_DATA waiting on echoes). */
        HAL_Delay(1U);
    }
}

/* ==========================================================================
 *  CLOCK CONFIGURATION
 * ==========================================================================
 *  HSI (16MHz) -> PLL -> SYSCLK = 64MHz, AHB = 64MHz, APB = 64MHz.
 *  (Matches the default CubeMX "Nucleo-G071RB max speed" configuration.)
 * ==========================================================================*/
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM       = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN       = 8;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLR       = RCC_PLLR_DIV2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                 | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ==========================================================================
 *  GPIO INIT
 * ==========================================================================*/
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* --- Ultrasonic TRIG outputs (push-pull) --- */
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pin = US_FL_TRIG_PIN;
    HAL_GPIO_Init(US_FL_TRIG_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = US_FR_TRIG_PIN;
    HAL_GPIO_Init(US_FR_TRIG_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = US_RL_TRIG_PIN;
    HAL_GPIO_Init(US_RL_TRIG_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = US_RR_TRIG_PIN;
    HAL_GPIO_Init(US_RR_TRIG_PORT, &GPIO_InitStruct);

    /* --- Ultrasonic ECHO inputs (EXTI, both edges) --- */
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;

    GPIO_InitStruct.Pin = US_FL_ECHO_PIN;
    HAL_GPIO_Init(US_FL_ECHO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = US_FR_ECHO_PIN;
    HAL_GPIO_Init(US_FR_ECHO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = US_RL_ECHO_PIN;
    HAL_GPIO_Init(US_RL_ECHO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = US_RR_ECHO_PIN;
    HAL_GPIO_Init(US_RR_ECHO_PORT, &GPIO_InitStruct);

    /* --- Buzzer + LEDs (push-pull outputs) --- */
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pin = BUZZER_PIN;
    HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LED_GREEN_PIN;
    HAL_GPIO_Init(LED_GREEN_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LED_RED_PIN;
    HAL_GPIO_Init(LED_RED_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LED_HEARTBEAT_PIN;
    HAL_GPIO_Init(LED_HEARTBEAT_PORT, &GPIO_InitStruct);

    /* --- Push button (parking mode toggle), active-low, EXTI falling --- */
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Pin  = BTN_PARKING_MODE_PIN;
    HAL_GPIO_Init(BTN_PARKING_MODE_PORT, &GPIO_InitStruct);

    /* --- Servo PWM pin (alternate function, TIM3_CH1) --- */
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    GPIO_InitStruct.Pin       = SERVO_PWM_PIN;
    HAL_GPIO_Init(SERVO_PWM_PORT, &GPIO_InitStruct);

    /* --- I2C1 pins (LCD) --- */
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_I2C1;
    GPIO_InitStruct.Pin       = LCD_I2C_SCL_PIN;
    HAL_GPIO_Init(LCD_I2C_SCL_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin       = LCD_I2C_SDA_PIN;
    HAL_GPIO_Init(LCD_I2C_SDA_PORT, &GPIO_InitStruct);

    /* --- USART2 pins (debug VCP) --- */
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
    GPIO_InitStruct.Pin       = UART_DEBUG_TX_PIN;
    HAL_GPIO_Init(UART_DEBUG_TX_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin       = UART_DEBUG_RX_PIN;
    HAL_GPIO_Init(UART_DEBUG_RX_PORT, &GPIO_InitStruct);
}

/* ==========================================================================
 *  I2C1 INIT (LCD backpack, 100kHz standard mode)
 * ==========================================================================*/
static void MX_I2C1_Init(void)
{
    __HAL_RCC_I2C1_CLK_ENABLE();

    hi2c1.Instance             = I2C1;
    hi2c1.Init.Timing          = 0x10909CECU; /* 100kHz @ 64MHz PCLK1 (CubeMX-calculated) */
    hi2c1.Init.OwnAddress1     = 0;
    hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2     = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ==========================================================================
 *  TIM2 INIT - free-running 1MHz microsecond timer (used for echo timing)
 * ==========================================================================*/
static void MX_TIM2_Init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* NOTE: TIM2 on STM32G071 is a 32-bit general-purpose counter, so it
     * can run free for ~71 minutes at 1MHz before wrapping - more than
     * sufficient headroom for a single ECHO_TIMEOUT_US (30ms) window. */
    htim2.Instance               = TIM2;
    htim2.Init.Prescaler         = (uint32_t)((SystemCoreClock / 1000000U) - 1U);
    htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim2.Init.Period            = 0xFFFFFFFFU;
    htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ==========================================================================
 *  TIM3 INIT - servo PWM, 50Hz (20ms period), 1us resolution
 * ==========================================================================*/
static void MX_TIM3_Init(void)
{
    __HAL_RCC_TIM3_CLK_ENABLE();

    TIM_OC_InitTypeDef sConfigOC = {0};

    htim3.Instance               = TIM3;
    htim3.Init.Prescaler         = (uint32_t)((SystemCoreClock / 1000000U) - 1U); /* 1us tick */
    htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim3.Init.Period            = SERVO_PWM_PERIOD_US - 1U; /* 20000us -> 50Hz */
    htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode     = TIM_OCMODE_PWM1;
    sConfigOC.Pulse       = 1500U; /* 1500us = 90 degrees, safe startup position */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ==========================================================================
 *  USART2 INIT - debug telemetry, 115200 8N1
 * ==========================================================================*/
static void MX_USART2_UART_Init(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();

    huart2.Instance          = USART2;
    huart2.Init.BaudRate      = UART_DEBUG_BAUDRATE;
    huart2.Init.WordLength    = UART_WORDLENGTH_8B;
    huart2.Init.StopBits      = UART_STOPBITS_1;
    huart2.Init.Parity        = UART_PARITY_NONE;
    huart2.Init.Mode          = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl     = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling  = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ==========================================================================
 *  NVIC INIT - EXTI priorities
 * ==========================================================================*/
static void MX_NVIC_Init(void)
{
    /* Echo lines: high-ish priority so pulse-width timing stays accurate */
    HAL_NVIC_SetPriority(EXTI0_1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

/* ==========================================================================
 *  EXTI CALLBACKS (invoked from stm32g0xx_it.c via HAL_GPIO_EXTI_Callback)
 * ==========================================================================*/
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == US_FL_ECHO_PIN)      ultrasonic_exti_handler(US_SENSOR_FRONT_LEFT, true);
    else if (GPIO_Pin == US_FR_ECHO_PIN) ultrasonic_exti_handler(US_SENSOR_FRONT_RIGHT, true);
    else if (GPIO_Pin == US_RL_ECHO_PIN) ultrasonic_exti_handler(US_SENSOR_REAR_LEFT, true);
    else if (GPIO_Pin == US_RR_ECHO_PIN) ultrasonic_exti_handler(US_SENSOR_REAR_RIGHT, true);
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == US_FL_ECHO_PIN)      ultrasonic_exti_handler(US_SENSOR_FRONT_LEFT, false);
    else if (GPIO_Pin == US_FR_ECHO_PIN) ultrasonic_exti_handler(US_SENSOR_FRONT_RIGHT, false);
    else if (GPIO_Pin == US_RL_ECHO_PIN) ultrasonic_exti_handler(US_SENSOR_REAR_LEFT, false);
    else if (GPIO_Pin == US_RR_ECHO_PIN) ultrasonic_exti_handler(US_SENSOR_REAR_RIGHT, false);
    else if (GPIO_Pin == BTN_PARKING_MODE_PIN) parking_app_button_isr(&s_app);
}

/* ==========================================================================
 *  ERROR HANDLER
 * ==========================================================================*/
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
        /* Fast-blink the on-board heartbeat LED to signal a fatal init
         * failure that occurred before the FSM/ERROR state was reachable
         * (e.g. clock configuration failure). */
        HAL_GPIO_TogglePin(LED_HEARTBEAT_PORT, LED_HEARTBEAT_PIN);
        for (volatile uint32_t i = 0; i < 200000U; i++) { }
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add reporting of file/line here for debugging. */
}
#endif
