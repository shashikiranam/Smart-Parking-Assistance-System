/**
 ******************************************************************************
 * @file    startup_stm32g071xx.s
 * @brief   Cortex-M0+ startup code: stack init, vector table, Reset_Handler
 * @project Smart-Parking-Assistance-System
 * @board   STM32G071RBTX
 ******************************************************************************
 */

  .syntax unified
  .cpu cortex-m0plus
  .fpu softvfp
  .thumb

.global g_pfnVectors
.global Default_Handler

.word _sidata
.word _sdata
.word _edata
.word _sbss
.word _ebss

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  ldr r0, =_estack
  mov sp, r0

  /* Copy .data section from FLASH to RAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit

  /* Zero-fill the .bss section */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

  bl SystemInit
  bl __libc_init_array
  bl main
  bx lr
  .size Reset_Handler, .-Reset_Handler

  .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler

/* ==========================================================================
 *  VECTOR TABLE (STM32G071 - Cortex-M0+, 32 external interrupt lines)
 * ==========================================================================*/
  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
  .size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word 0
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler

  /* External Interrupts */
  .word WWDG_IRQHandler                 /* Window Watchdog              */
  .word PVD_VDDIO2_IRQHandler           /* PVD / VDDIO2                 */
  .word RTC_TAMP_IRQHandler             /* RTC / Tamper                 */
  .word FLASH_IRQHandler                /* Flash                        */
  .word RCC_IRQHandler                  /* RCC                          */
  .word EXTI0_1_IRQHandler              /* EXTI Line 0 and 1             */
  .word EXTI2_3_IRQHandler              /* EXTI Line 2 and 3             */
  .word EXTI4_15_IRQHandler             /* EXTI Line 4 to 15             */
  .word UCPD1_2_IRQHandler              /* UCPD1, UCPD2                 */
  .word DMA1_Channel1_IRQHandler        /* DMA1 Channel 1                */
  .word DMA1_Channel2_3_IRQHandler      /* DMA1 Channel 2 and 3          */
  .word DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler /* DMA1 Ch4-7, DMAMUX1 overrun */
  .word ADC1_COMP_IRQHandler            /* ADC1, COMP1, COMP2            */
  .word TIM1_BRK_UP_TRG_COM_IRQHandler  /* TIM1 Break/Update/Trig/Comm  */
  .word TIM1_CC_IRQHandler              /* TIM1 Capture Compare          */
  .word TIM2_IRQHandler                 /* TIM2                          */
  .word TIM3_IRQHandler                 /* TIM3                          */
  .word TIM6_DAC_LPTIM1_IRQHandler      /* TIM6, DAC, LPTIM1             */
  .word TIM7_LPTIM2_IRQHandler          /* TIM7, LPTIM2                  */
  .word TIM14_IRQHandler                /* TIM14                         */
  .word TIM15_IRQHandler                /* TIM15                         */
  .word TIM16_IRQHandler                /* TIM16                         */
  .word TIM17_IRQHandler                /* TIM17                         */
  .word I2C1_IRQHandler                 /* I2C1                          */
  .word I2C2_IRQHandler                 /* I2C2                          */
  .word SPI1_IRQHandler                 /* SPI1                          */
  .word SPI2_IRQHandler                 /* SPI2                          */
  .word USART1_IRQHandler               /* USART1                        */
  .word USART2_IRQHandler               /* USART2                        */
  .word USART3_4_LPUART1_IRQHandler     /* USART3, USART4, LPUART1       */
  .word CEC_IRQHandler                  /* CEC                           */

/* ==========================================================================
 *  WEAK ALIASES - default all unimplemented handlers to an infinite loop
 * ==========================================================================*/
  .weak      NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak      HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak      SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak      PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak      SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler

  .weak      WWDG_IRQHandler
  .thumb_set WWDG_IRQHandler,Default_Handler
  .weak      PVD_VDDIO2_IRQHandler
  .thumb_set PVD_VDDIO2_IRQHandler,Default_Handler
  .weak      RTC_TAMP_IRQHandler
  .thumb_set RTC_TAMP_IRQHandler,Default_Handler
  .weak      FLASH_IRQHandler
  .thumb_set FLASH_IRQHandler,Default_Handler
  .weak      RCC_IRQHandler
  .thumb_set RCC_IRQHandler,Default_Handler
  .weak      EXTI0_1_IRQHandler
  .thumb_set EXTI0_1_IRQHandler,Default_Handler
  .weak      EXTI2_3_IRQHandler
  .thumb_set EXTI2_3_IRQHandler,Default_Handler
  .weak      EXTI4_15_IRQHandler
  .thumb_set EXTI4_15_IRQHandler,Default_Handler
  .weak      UCPD1_2_IRQHandler
  .thumb_set UCPD1_2_IRQHandler,Default_Handler
  .weak      DMA1_Channel1_IRQHandler
  .thumb_set DMA1_Channel1_IRQHandler,Default_Handler
  .weak      DMA1_Channel2_3_IRQHandler
  .thumb_set DMA1_Channel2_3_IRQHandler,Default_Handler
  .weak      DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler
  .thumb_set DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler,Default_Handler
  .weak      ADC1_COMP_IRQHandler
  .thumb_set ADC1_COMP_IRQHandler,Default_Handler
  .weak      TIM1_BRK_UP_TRG_COM_IRQHandler
  .thumb_set TIM1_BRK_UP_TRG_COM_IRQHandler,Default_Handler
  .weak      TIM1_CC_IRQHandler
  .thumb_set TIM1_CC_IRQHandler,Default_Handler
  .weak      TIM2_IRQHandler
  .thumb_set TIM2_IRQHandler,Default_Handler
  .weak      TIM3_IRQHandler
  .thumb_set TIM3_IRQHandler,Default_Handler
  .weak      TIM6_DAC_LPTIM1_IRQHandler
  .thumb_set TIM6_DAC_LPTIM1_IRQHandler,Default_Handler
  .weak      TIM7_LPTIM2_IRQHandler
  .thumb_set TIM7_LPTIM2_IRQHandler,Default_Handler
  .weak      TIM14_IRQHandler
  .thumb_set TIM14_IRQHandler,Default_Handler
  .weak      TIM15_IRQHandler
  .thumb_set TIM15_IRQHandler,Default_Handler
  .weak      TIM16_IRQHandler
  .thumb_set TIM16_IRQHandler,Default_Handler
  .weak      TIM17_IRQHandler
  .thumb_set TIM17_IRQHandler,Default_Handler
  .weak      I2C1_IRQHandler
  .thumb_set I2C1_IRQHandler,Default_Handler
  .weak      I2C2_IRQHandler
  .thumb_set I2C2_IRQHandler,Default_Handler
  .weak      SPI1_IRQHandler
  .thumb_set SPI1_IRQHandler,Default_Handler
  .weak      SPI2_IRQHandler
  .thumb_set SPI2_IRQHandler,Default_Handler
  .weak      USART1_IRQHandler
  .thumb_set USART1_IRQHandler,Default_Handler
  .weak      USART2_IRQHandler
  .thumb_set USART2_IRQHandler,Default_Handler
  .weak      USART3_4_LPUART1_IRQHandler
  .thumb_set USART3_4_LPUART1_IRQHandler,Default_Handler
  .weak      CEC_IRQHandler
  .thumb_set CEC_IRQHandler,Default_Handler
