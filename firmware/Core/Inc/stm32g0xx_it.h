/**
 ******************************************************************************
 * @file    stm32g0xx_it.h
 * @brief   Interrupt Service Routine (ISR) declarations
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#ifndef STM32G0XX_IT_H
#define STM32G0XX_IT_H

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void EXTI0_1_IRQHandler(void);
void EXTI2_3_IRQHandler(void);
void EXTI4_15_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* STM32G0XX_IT_H */
