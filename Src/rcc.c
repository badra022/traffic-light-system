/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 7/1/2021				*************/
/*********** Version	: V02					*************/
/************************************************************/
#include "stdtypes.h"
#include "macros.h"
#include "port.h"

#include "rcc_config.h"
#include "rcc.h"

static u32 system_clock = 16000000U;		/* default */
#define SCB_BASE_ADDRESS			0xE000ED00
#define SCB_CPACR			*(volatile u32*)(SCB_BASE_ADDRESS + 0x88)
/************************************************************/
/*						FUNCTION DEFINITIONS				*/
/************************************************************/

void RCC_voidEnableClock(u8 bus, u8 pin){
	if(pin <= 31)
	{
		/* the inserted pin number is accessible in 32bit registers */
		/* now depending on the bus selected, we will select the register we want */
		switch(bus)
		{
			case RCC_AHB1:	SET_BIT(RCC_AHB1ENR, pin);	break;
			case RCC_AHB2:	SET_BIT(RCC_AHB2ENR, pin);	break;
			case RCC_AHB3:	SET_BIT(RCC_AHB3ENR, pin);	break;
			case RCC_APB1:	SET_BIT(RCC_APB1ENR, pin);	break;
			case RCC_APB2:	SET_BIT(RCC_APB2ENR, pin);	break;
		}
	}
}


void RCC_initSystemClock(void){

	/* use FPU */
	SCB_CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */

	/* set general RCC configurations */
	RCC_CFGR |= (AHB_APB1_DIV_FACTOR << 10) | (AHB_APB2_DIV_FACTOR << 13) | (SYSCLK_AHB_DIV_FACTOR << 4);

	RCC_CIR |= (LSI_INTERRUPT_ENABLE << 8)
			|  (LSE_INTERRUPT_ENABLE << 9)
			|  (HSI_INTERRUPT_ENABLE << 10)
			|  (HSE_INTERRUPT_ENABLE << 11)
			|  (PLL_INTERRUPT_ENABLE << 12)
			|  (PLLI2S_INTERRUPT_ENABLE << 13);

#if LSI_RC == ENABLE
	/* enable LSI */
	SET_BIT(RCC_CSR, 0);
	while(!GET_BIT(RCC_CSR, 1)){}
#endif

#if LSE_CRYSTAL == ENABLE
	/* enable LSE and set bypass config */
	RCC_BDCR |= (1 << 0) | (LSE_BYPASS << 2);
	while(!GET_BIT(RCC_BDCR, 1)){}
#endif

#if SYSTEM_CLOCK_SRC == HSI_SRC
	/* set the trimming */
	RCC_CR |= TRIMMING_VALUE << 3;

	/* enable HSI clock */
	SET_BIT(RCC_CR, 0);
	while(!GET_BIT(RCC_CR, 1)){}

	/* select HSI as system clock */
	RCC_CFGR &= ~0x03;
	RCC_CFGR |= HSI_SRC;

#elif SYSTEM_CLOCK_SRC == HSE_SRC
	/* set clock security system if enabled */
	RCC_CIR |= CLOCK_SECURITY_SYSTEM << 19;

	/* set bypass mode if enabled */
	RCC_CR |= BYPASS << 18;

	/* enable HSE clock */
	SET_BIT(RCC_CR, 16);
	while(!GET_BIT(RCC_CR, 17)){}

	/* select HSE as system clock */
	RCC_CFGR &= ~0x03;
	RCC_CFGR |= HSE_SRC;

#elif SYSTEM_CLOCK_SRC == PLL_SRC
	RCC_PLLCFGR |= (PLL_Q_DIV_FACTOR << 24) | (PLL_M_DIV_FACTOR << 0)
			| (PLL_P_DIV_FACTOR << 16) | (PLL_N_MUL_FACTOR << 6)

#if PLL_PLLI2S_CLOCK_SRC == HSI_SRC
	/* set the trimming */
	RCC_CR |= TRIMMING_VALUE << 3;

	/* enable HSI clock */
	SET_BIT(RCC_CR, 0);
	while(!GET_BIT(RCC_CR, 1)){}

	/* select HSI as system clock */
	RCC_CFGR &= ~0x03;
	RCC_CFGR |= HSI_SRC;

	/* select HSI as clock src of the PLL */
	CLR_BIT(RCC_PLLCFGR, 22);
#elif PLL_PLLI2S_CLOCK_SRC == HSE_SRC
	/* set clock security system if enabled */
	RCC_CIR |= CLOCK_SECURITY_SYSTEM << 19;

	/* set bypass mode if enabled */
	RCC_CR |= BYPASS << 18;

	/* enable HSE clock */
	SET_BIT(RCC_CR, 16);
	while(!GET_BIT(RCC_CR, 17)){}

	/* select HSE as system clock */
	RCC_CFGR &= ~0x03;
	RCC_CFGR |= HSE_SRC;

	/* select HSE as clock src of the PLL */
	SET_BIT(RCC_PLLCFGR, 22);
#endif

	/* enable PLL */
	SET_BIT(RCC_CR, 24);
	while(!GET_BIT(RCC_CR, 25)){}

	/* select PLL as system clock */
	RCC_CFGR &= ~0x03;
	RCC_CFGR |= PLL_SRC;

#endif

#if PLLI2S == ENABLE
	/* set Factors of PPLI2S */
	RCC_PLLI2SCFGR = (PLLI2S_R_DIV_FACTOR << 28) | (PLLI2S_N_MUL_FACTOR << 6);
	RCC_PLLCFGR &= ~0x3F;
	RCC_PLLCFGR |= PLLI2S_M_DIV_FACTOR;

	/* enable PPLI2S */
	SET_BIT(RCC_CR, 26);
	while(!GET_BIT(RCC_CR, 27)){}
#endif

#if I2S_CLOCK == ENABLE
#if I2S_CLOCK_SRC == PLLI2S_SRC
	/* select PLLI2S as I2S clock src */
	CLR_BIT(RCC_CFGR, 23);
#elif I2S_CLOCK_SRC == I2S_CKIN
	/* select I2S_CKIN as I2S clock src */
	SET_BIT(RCC_CFGR, 23);
#endif
#endif

#if RTC_CLOCK == ENABLE
#if RTC_CLOCK_SRC == LSI_RC_SRC
	/* select RTC clock src */
	RCC_BDCR &= ~(0x03 << 8);
	RCC_BDCR |= 0x02 << 8;

#elif RTC_CLOCK_SRC == LSE_SRC
	/* select RTC clock src */
	RCC_BDCR &= ~(0x03 << 8);
	RCC_BDCR |= 0x01 << 8;

#elif RTC_CLOCK_SRC == HSE_SRC
	/* select RTC clock src */
	RCC_BDCR &= ~(0x03 << 8);
	RCC_BDCR |= 0x03 << 8;

	/* set HSE prescaler for RTC */
	RCC_CFGR |= HSE_RTC_PRESCALER << 16;

#endif
	/* enable RTC clock */
	SET_BIT(RCC_BDCR, 15);

#endif


#if MCO1_PA8_OUTPUT_PIN == ENABLE
	RCC_CFGR |= (MCO1_SRC << 21) | (MCO1_SRC_PRESCALER << 24);
#endif

#if MCO2_PC9_OUTPUT_PIN == ENABLE
	RCC_CFGR |= (MCO2_SRC << 30) | (MCO2_SRC_PRESCALER << 27);
#endif
}


void RCC_voidDisableClock(u8 bus, u8 pin){
	if(pin <= 31)
	{
		/* the inserted pin number is accessible in 32bit registers */
		/* now depending on the bus selected, we will select the register we want */
		switch(bus)
		{
			case RCC_AHB1:	CLR_BIT(RCC_AHB1ENR, pin);	break;
			case RCC_AHB2:	CLR_BIT(RCC_AHB2ENR, pin);	break;
			case RCC_AHB3:	CLR_BIT(RCC_AHB3ENR, pin);	break;
			case RCC_APB1:	CLR_BIT(RCC_APB1ENR, pin);	break;
			case RCC_APB2:	CLR_BIT(RCC_APB2ENR, pin);	break;
		}
	}
}

static void updateSystemClock(void){
	u32 tmp, pllsource, pllm, pllp;
	float32  pllvco;
	/* Get SYSCLK source -------------------------------------------------------*/
	tmp = RCC_CFGR & (0x03 << 2);

	switch (tmp)
	{
	case 0x00:  /* HSI used as system clock source */
		system_clock = HSI_FREQ;
	  break;
	case 0x04:  /* HSE used as system clock source */
		system_clock = HSE_FREQ;
	  break;
	case 0x08:  /* PLL used as system clock source */

	  /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
		 SYSCLK = PLL_VCO / PLL_P
		 */
	  pllsource = (RCC_PLLCFGR & (0x01 << 22)) >> 22;
	  pllm = RCC_PLLCFGR & 0x3F;

	  if (pllsource != HSI_SRC)
	  {
		/* HSE used as PLL clock source */
		pllvco = (HSE_FREQ / pllm) * ((RCC_PLLCFGR & (0x1FF << 6)) >> 6);
	  }
	  else
	  {
		/* HSI used as PLL clock source */
		pllvco = (HSI_FREQ / pllm) * ((RCC_PLLCFGR & (0x1FF << 6)) >> 6);
	  }

	  pllp = (((RCC_PLLCFGR & (0x03 << 16)) >>16) + 1 ) *2;
	  system_clock = pllvco/pllp;
	  break;
	default:
		system_clock = HSI_FREQ;
	  break;
	}
	/* Compute HCLK frequency --------------------------------------------------*/
	/* Get HCLK prescaler */
	if(SYSCLK_AHB_DIV_FACTOR != SYSCLK_AHB_NOT_DIVIDED){
	  tmp = SYSCLK_AHB_DIV_FACTOR - 7;

	  /* HCLK frequency */
	  system_clock >>= tmp;
	}
}

u32 RCC_u32GetSystemClock(void){
	updateSystemClock();
	return system_clock;
}
