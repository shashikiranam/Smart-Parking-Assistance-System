/**
 ******************************************************************************
 * @file    stm32g0xx_it.c
 * @brief   Interrupt Service Routine (ISR) implementations
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 * All routines here are thin: they call into HAL_GPIO_EXTI_IRQHandler(),
 * which in turn dispatches to HAL_GPIO_EXTI_Rising_Callback() /
 * HAL_GPIO_EXTI_Falling_Callback() defined in main.c. This keeps hardware
 * vector wiring separate from application interrupt logic.
 ******************************************************************************
 */

#include "main.h"
#include "stm32g0xx_it.h"
#include "pin_map.h"

/* ==========================================================================
 *  CORTEX-M0+ CORE HANDLERS
 * ==========================================================================*/

void NMI_Handler(void)
{
    while (1) { }
}

void HardFault_Handler(void)
{
    /* A hard fault indicates a serious firmware defect (bad pointer,
     * stack overflow, etc). Latch the heartbeat LED solid and halt rather
     * than attempt to recover into a possibly-corrupted FSM state. */
    HAL_GPIO_WritePin(LED_HEARTBEAT_PORT, LED_HEARTBEAT_PIN, GPIO_PIN_SET);
    while (1) { }
}

void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

/* ==========================================================================
 *  EXTI HANDLERS
 * ==========================================================================
 *  Pin-to-line mapping used by this project (see pin_map.h):
 *    EXTI0_1_IRQn  -> US_FL_ECHO (PA1, line 1)
 *    EXTI4_15_IRQn -> US_FR_ECHO (PA8, line 8), US_RL_ECHO (PB11, line 11),
 *                     US_RR_ECHO (PB12, line 12), BTN_PARKING_MODE (PC13,
 *                     line 13)
 * ==========================================================================*/

void EXTI0_1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(US_FL_ECHO_PIN);
}

void EXTI2_3_IRQHandler(void)
{
    /* No sensor/button lines mapped to 2 or 3 in this design (reserved for
     * USART2 TX/RX alternate function on PA2/PA3), handler kept for
     * completeness / future expansion. */
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI4_15_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(US_FR_ECHO_PIN);
    HAL_GPIO_EXTI_IRQHandler(US_RL_ECHO_PIN);
    HAL_GPIO_EXTI_IRQHandler(US_RR_ECHO_PIN);
    HAL_GPIO_EXTI_IRQHandler(BTN_PARKING_MODE_PIN);
}
