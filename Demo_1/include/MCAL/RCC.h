/*
 * RCC.h
 * Created on: Feb 20, 2024
 * Author: Amina Tawfik
 */

#ifndef RCC_H
#define RCC_H

#include "std_types.h"
#include "Errors.h"

/************* Clock Options ***************/
#define CLK_HSI         0x0000000200000001
#define CLK_HSE_OSC     0x0002000000010000
#define CLK_HSE_EXT     0x0002000000050000
#define CLK_PLL         0x0200000001000000
#define CLK_PLLI2S      0x0800000004000000

/************* System Clock Options ***************/
#define SYS_CLK_HSI     0x000000000
#define SYS_CLK_HSE     0x000000001
#define SYS_CLK_PLL     0x000000002

/************* P values to configure PLL ***************/
#define P_2             0x00000000
#define P_4             0x00010000
#define P_6             0x00020000
#define P_8             0x00030000

/************* AHB1 Peripheral Options *******************/
#define AHB1EN_GPIOA    0x0000000000000001
#define AHB1EN_GPIOB    0x0000000000000002
#define AHB1EN_GPIOC    0x0000000000000004
#define AHB1EN_GPIOD    0x0000000000000008
#define AHB1EN_GPIOE    0x0000000000000010
#define AHB1EN_GPIOH    0x0000000000000080
#define AHB1EN_CRC      0x0000000000001000
#define AHB1EN_DMA1     0x0000000000020000
#define AHB1EN_DMA2     0x0000000000040000

/************* APB1 Peripheral Options *******************/

#define APB1EN_TIM2     0x0000000100000001
#define APB1EN_TIM3     0x0000000100000002
#define APB1EN_TIM4     0x0000000100000004
#define APB1EN_TIM5     0x0000000100000008
#define APB1EN_WWDG     0x0000000100000800
#define APB1EN_SPI2     0x0000000100004000
#define APB1EN_SPI3     0x0000000100008000
#define APB1EN_USART2   0x0000000100020000
#define APB1EN_I2C1     0x0000000100200000
#define APB1EN_I2C2     0x0000000100400000
#define APB1EN_I2C3     0x0000000100800000
#define APB1EN_PWR      0x0000000110000000

/************** APB2 Peripheral Options *******************/

#define APB2EN_TIM1     0x0000000200000001
#define APB2EN_USART1   0x0000000200000010
#define APB2EN_USART6   0x0000000200000020
#define APB2EN_ADC1     0x0000000200000100
#define APB2EN_SDIO     0x0000000200000800
#define APB2EN_SPI1     0x0000000200001000
#define APB2EN_SPI4     0x0000000200002000
#define APB2EN_SYSCFG   0x0000000200004000
#define APB2EN_TIM9     0x0000000200010000
#define APB2EN_TIM10    0x0000000200020000
#define APB2EN_TIM11    0x0000000200040000

/************** AHB2 Peripheral Options *******************/
#define AHB2EN_OTGFS    0x0000000300000080

/********************** Bus Options **********************/
#define AHB1            0x00
#define APB1            0x01
#define APB2            0x02
#define AHB2            0x03

/********************** Clock Status Options **********************/
#define RCC_STAT_ON     0x00000001
#define RCC_STAT_OFF    0x00000000

/********************** AHB1 Prescaler Options **********************/
#define HPRE_NDIV       0x0000000000000000
#define HPRE_2          0x0000000000000080
#define HPRE_4          0x0000000000000090
#define HPRE_8          0x0000000000000050
#define HPRE_16         0x00000000000000B0
#define HPRE_64         0x00000000000000C0
#define HPRE_128        0x00000000000000D0
#define HPRE_256        0X00000000000000E0
#define HPRE_512        0x00000000000000F0


/********************** APB1 Prescaler Options **********************/
#define PPRE1_NDIV      0x0000000100000000
#define PPRE1_2         0x0000000100001000
#define PPRE1_4         0x0000000100001400
#define PPRE1_8         0X0000000100001800
#define PPRE1_16        0x0000000100001C00

/********************** APB2 Prescaler Options **********************/
#define PPRE2_NDIV      0x0000000200000000
#define PPRE2_2         0x0000000200008000
#define PPRE2_4         0x000000020000A000
#define PPRE2_8         0X000000020000C000
#define PPRE2_16        0x000000020000E000

/**
 * @brief Controls clock as ON or OFF
 *
 * This function controls the specified clock, turning it on or off.
 *
 * @param CLK_clock: Clock to control.
 * @param RCC_STAT: Status of the clock (RCC_STAT_ON or RCC_STAT_OFF).
 * @return ErrorStatus_t Returns OK if the operation is successful, otherwise returns an error status.
 *
 * Notes:
 * - You cannot turn off the system clock.
 * - You cannot turn off the PLL source clock if PLL is the system clock.
 */
ErrorStatus_t RCC_ControlClock(uint64_t CLK_clock, uint8_t RCC_STAT);

/**
 * @brief Selects system clock
 *
 * This function selects the system clock from the available options.
 *
 * @param SYS_CLK: System clock to select.
 * @return ErrorStatus_t Returns OK if the operation is successful, otherwise returns an error status.
 *
 * Note: You cannot select a turned-off clock as the system clock.
 */
ErrorStatus_t RCC_SelectSystemClock(uint32_t SYS_CLK);

/**
 * @brief Configures PLL main clock
 *
 * This function configures the main clock for the PLL.
 *
 * @param CLK_clock: Clock to use as PLL source (CLK_HSE_OSC, CLK_HSE_EXT, or CLK_HSI).
 * @param P: P value for PLL configuration (P_2, P_4, P_6, P_8).
 * @param M: M value for PLL configuration (2 <= M <= 63).
 * @param N: N value for PLL configuration (192 <= N <= 432).
 * @return ErrorStatus_t Returns OK if the operation is successful, otherwise returns an error status.
 *
 * Notes:
 * - Make sure PLL is turned off before using this function.
 * - P values are P_2, P_4, P_6, P_8.
 * - M ranges from 2 to 63.
 * - N ranges from 192 to 432.
 */
ErrorStatus_t RCC_ConfigurePLL(uint32_t CLK_clock, uint32_t P, uint16_t M, uint16_t N);

/**
 * @brief Enables peripheral
 *
 * This function enables the specified peripheral on the selected bus.
 *
 * @param Bus: Bus to which the peripheral belongs (AHB1, APB1, APB2, or AHB2).
 * @param BUSEN_peripheral: Desired peripheral to enable.
 * @return ErrorStatus_t Returns OK if the operation is successful, otherwise returns an error status.
 *
 * Note: Make sure the peripheral matches the selected bus.
 */
ErrorStatus_t RCC_EnablePeripheral(uint8_t Bus, uint64_t BUSEN_peripheral);

/**
 * @brief Disables peripheral
 *
 * This function disables the specified peripheral on the selected bus.
 *
 * @param Bus: Bus to which the peripheral belongs (AHB1, APB1, APB2, or AHB2).
 * @param BUSEN_peripheral: Desired peripheral to disable.
 * @return ErrorStatus_t Returns OK if the operation is successful, otherwise returns an error status.
 *
 * Note: Make sure the peripheral matches the selected bus.
 */
ErrorStatus_t RCC_DisablePeripheral(uint8_t Bus, uint64_t BUSEN_peripheral);

/**
 * @brief Sets AHB1 prescaler value
 *
 * This function sets the prescaler value for AHB1.
 *
 * @param HPRE_value: Prescaler value for AHB1.
 * @return ErrorStatus_t Returns OK if the operation is successful, otherwise returns an error status.
 */
ErrorStatus_t RCC_SetAHB1Prescaler(uint64_t HPRE_value);

/**
 * @brief Sets APB1 prescaler value
 *
 * This function sets the prescaler value for APB1.
 *
 * @param PPRE1_value: Prescaler value for APB1.
 * @return ErrorStatus_t Returns OK if the operation is successful, otherwise returns an error status.
 */
ErrorStatus_t RCC_SetAPB1Prescaler(uint64_t PPRE_value);

/**
 * @brief Sets APB2 prescaler value
 *
 * This function sets the prescaler value for APB2.
 *
 * @param PPRE2_value: Prescaler value for APB2.
 * @return ErrorStatus_t Returns OK if the operation is successful, otherwise returns an error status.
 */
ErrorStatus_t RCC_SetAPB2Prescaler(uint64_t PPRE_value);

#endif

