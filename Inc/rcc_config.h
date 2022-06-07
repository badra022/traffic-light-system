/*
 * rcc_config.h
 *
 *  Created on: Jun 1, 2022
 *      Author: Badra
 */

#ifndef INC_RCC_CONFIG_H_
#define INC_RCC_CONFIG_H_

/***************************** START DON'T CHANGE SECTION **************************************/
#define AHB_APB_NOT_DIVIDED	0
#define AHB_APB_DIV_BY_2	4
#define AHB_APB_DIV_BY_4	5
#define AHB_APB_DIV_BY_8	6
#define AHB_APB_DIV_BY_16	7

#define SYSCLK_AHB_NOT_DIVIDED		0
#define SYSCLK_AHB_DIV_2			8
#define SYSCLK_AHB_DIV_4			9
#define SYSCLK_AHB_DIV_8			10
#define SYSCLK_AHB_DIV_16			11
#define SYSCLK_AHB_DIV_64			12
#define SYSCLK_AHB_DIV_128			13
#define SYSCLK_AHB_DIV_256			14
#define SYSCLK_AHB_DIV_512			15

#define MCO_NO_DIVISION				0
#define MCO_DIV_2					4
#define MCO_DIV_3					5
#define MCO_DIV_4					6
#define MCO_DIV_5					7

#define MCO_HSE_SRC					2
#define MCO_PLL_SRC					3
#define MCO_SYSTEM_CLOCK_SRC		0
#define MCO_PLLI2S_SRC				1
#define MCO_HSI_SRC					0
#define MCO_LSE_SRC					1

#define HSI_SRC			0
#define HSE_SRC			1
#define PLL_SRC			2
#define LSI_RC_SRC		3
#define LSE_CRYSTAL_SRC	4
#define PLLI2S_SRC		5

#define I2S_CKIN		1

#define HSI_FREQ		16000000U
#define HSE_FREQ		25000000U
/***************************** END DON'T CHANGE SECTION **************************************/


/*
 * HSI_SRC
 * HSE_SRC
 * PLL_SRC
 */
#define SYSTEM_CLOCK_SRC		HSI_SRC

/*
 * ENABLED
 * DISABLED
 */
#define LSI_RC					DISABLE
#define LSE_CRYSTAL				DISABLE
#define RTC_CLOCK				DISABLE
#define MCO1_PA8_OUTPUT_PIN		DISABLE
#define MCO2_PC9_OUTPUT_PIN		DISABLE
#define I2S_CLOCK				DISABLE
#define PLLI2S					DISABLE
#define LSI_INTERRUPT_ENABLE	DISABLE
#define LSE_INTERRUPT_ENABLE	DISABLE
#define HSI_INTERRUPT_ENABLE	DISABLE
#define HSE_INTERRUPT_ENABLE	DISABLE
#define PLL_INTERRUPT_ENABLE	DISABLE
#define PLLI2S_INTERRUPT_ENABLE	DISABLE
/* Note: flags must be cleared in the ISR by writing 1 to the corresponding interrupt clear bit*/

/*
 * AHB_APB_NOT_DIVIDED
 * AHB_APB_DIV_BY_2
 * AHB_APB_DIV_BY_4
 * AHB_APB_DIV_BY_8
 * AHB_APB_DIV_BY_16
 */
#define AHB_APB1_DIV_FACTOR		AHB_APB_NOT_DIVIDED
#define AHB_APB2_DIV_FACTOR		AHB_APB_DIV_BY_8

/*
 * SYSCLK_AHB_NOT_DIVIDED
 * SYSCLK_AHB_DIV_2
 * SYSCLK_AHB_DIV_4
 * SYSCLK_AHB_DIV_8
 * SYSCLK_AHB_DIV_16
 * SYSCLK_AHB_DIV_64
 * SYSCLK_AHB_DIV_128
 * SYSCLK_AHB_DIV_256
 * SYSCLK_AHB_DIV_512
 */
#define SYSCLK_AHB_DIV_FACTOR	SYSCLK_AHB_NOT_DIVIDED

#if (SYSTEM_CLOCK_SRC == HSI_SRC)
/* 0 1 2 .... 31 */
#define TRIMMING_VALUE		0
#endif

#if SYSTEM_CLOCK_SRC == PLLI2S_SRC
#warning "PLLI2S cannot be a source for system clock "
#endif

#if MCO1_PA8_OUTPUT_PIN == ENABLE
/*
 * MCO_HSI_SRC
 * MCO_LSE_SRC
 * MCO_HSE_SRC
 * MCO_PLL_SRC
 */
#define MCO1_SRC				MCO_HSI_SRC

/*
 * MCO_NO_DIVISION
 * MCO_DIV_2
 * MCO_DIV_3
 * MCO_DIV_4
 * MCO_DIV_5
 */
#define MCO1_SRC_PRESCALER		MCO_NO_DIVISION

#endif

#if MCO2_PC9_OUTPUT_PIN == ENABLED
/*
 * MCO_SYSTEM_CLOCK_SRC
 * MCO_PLLI2S_SRC
 * MCO_HSE_SRC
 * MCO_PLL_SRC
 */
#define MCO2_SRC				MCO_SYSTEM_CLOCK_SRC

/*
 * MCO_NO_DIVISION
 * MCO_DIV_2
 * MCO_DIV_3
 * MCO_DIV_4
 * MCO_DIV_5
 */
#define MCO2_SRC_PRESCALER		MCO_NO_DIVISION

#endif


#if SYSTEM_CLOCK_SRC != HSI_SRC
#define	CLOCK_SECURITY_SYSTEM		ENABLE
#endif

#if (SYSTEM_CLOCK_SRC == LSI_RC_SRC) || (SYSTEM_CLOCK_SRC == LSE_CRYSTAL_SRC)
#warning "LSI_RC_SRC and LSI_CRYSTAL_SRC are secondary clocks, cannot be the source of the system clock"
#endif

#if (LSI_RC == ENABLE) && (LSE_CRYSTAL == ENABLE)
#warning "Only one secondary clock shall be enabled, LSI_RC or LSE_CRYSTAL"
#endif

#if LSE_CRYSTAL == ENABLE
#define	LSE_BYPASS		ENABLE
#endif

#if SYSTEM_CLOCK_SRC == HSE_SRC
#define BYPASS 		TRUE
#endif

#if SYSTEM_CLOCK_SRC == PLL_SRC

/*
 * HSI_SRC
 * HSE_SRC
 */
#define	PLL_PLLI2S_CLOCK_SRC	HSI_SRC


/* 2 3 4 .... 15 */
#define PLL_Q_DIV_FACTOR		2

/* 2 3 4 5 .... 63 */
#define PLL_M_DIV_FACTOR		2

/* 2 4 6 8 */
#define PLL_P_DIV_FACTOR		2

/* 192 193 194 .... 432 */
#define PLL_N_MUL_FACTOR		192

#endif

#if (PLLI2S == ENABLE)
/* 2 3 4 ... 7 */
#define PLLI2S_R_DIV_FACTOR		2
/* constraint: PLLI2S_M_DIV_FACTOR must equal PLL_M_DIV_FACTOR */
#define PLLI2S_M_DIV_FACTOR		2
/* 192 193 194 ... 432 */
#define PLLI2S_N_MUL_FACTOR		192

#ifdef PLL_M_DIV_FACTOR
#if PLLI2S_M_DIV_FACTOR != PLL_M_DIV_FACTOR
#warning "PLLI2S_M_DIV_FACTOR must equal PLL_M_DIV_FACTOR"
#endif
#endif

#endif

#if (RTC_CLOCK == ENABLE)

/*
 * LSI_RC_SRC
 * LSE_SRC
 * HSE_SRC
 */
#define RTC_CLOCK_SRC			LSI_RC_SRC

#if (RTC_CLOCK_SRC == HSI_SRC) || (RTC_CLOCK_SRC == PLL_SRC)
#warning "HSI and PLL cannot be sources for RTC clock"
#endif

#if RTC_CLOCK_SRC == HSE_SRC
/* 2 3 4 5 ... 31 */
/* this prescaler is to ensure that HSE/prescaler = 1MHZ */
#define HSE_RTC_PRESCALER		2
#endif

#endif

#if I2S_CLOCK == ENABLE
/*
 * PLLI2S_SRC
 * I2S_CKIN
 */
#define I2S_CLOCK_SRC		PLLI2S_SRC
#endif

#if (I2S_CLOCK == ENABLE) && (I2S_CLOCK_SRC != PLLI2S_SRC) && (I2S_CLOCK_SRC != I2S_CKIN)
#warning "I2S source must be specified"
#endif


#endif /* INC_RCC_CONFIG_H_ */
