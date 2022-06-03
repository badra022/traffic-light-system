/*
 * port.h
 *
 *  Created on: Jun 1, 2022
 *      Author: Badra
 */

#ifndef INC_PORT_H_
#define INC_PORT_H_


/**********************************************************************************************/
/*								COMMON MACROS													*/
/**********************************************************************************************/
#define CRITICAL_SECTION_START()		__asm("CPSID I")
#define CRITICAL_SECTION_END()			__asm("CPSIE I")
#define SVC_INTERRUPT_TRIGGER()			__asm("SVC 0")

/**********************************************************************************************/
/*								REGISTERS													*/
/**********************************************************************************************/

/*****************************GPIO**********************************************/
#define GPIOA_ADDRESS		0x40020000
#define GPIOB_ADDRESS		0x40020400
#define GPIOC_ADDRESS		0x40020800
#define GPIOD_ADDRESS		0x40020C00
#define GPIOE_ADDRESS		0x40021000
#define GPIOF_ADDRESS		0x40021400
#define GPIOG_ADDRESS		0x40021800
#define GPIOH_ADDRESS		0x40021C00
#define GPIOI_ADDRESS		0x40022000
#define GPIOJ_ADDRESS		0x40022400
#define GPIOK_ADDRESS		0x40022800

/****************************TIMERS**********************************************/
typedef struct{
	u32 CR1;
	u32 CR2;
	u32 SMCR;
	u32 DIER;
	u32 SR;
	u32 EGR;
	u32 CCMR1;
	u32 CCMR2;
	u32 CCER;
	u32 CNT;
	u32 PSC;
	u32 ARR;
	u32 RESERVED1;
	u32 CCR1;
	u32 CCR2;
	u32 CCR3;
	u32 CCR4;
	u32 RESERVED2;
	u32 DCR;
	u32 DMAR;
}Gptim2_5_dtype;

#define TIM2_BASE_ADDRESS	(0x40000000)
#define TIM3_BASE_ADDRESS	(0x40000400)
#define TIM4_BASE_ADDRESS	(0x40000800)
#define TIM5_BASE_ADDRESS	(0x40000C00)



/*******************************INTERRUPT CONTROLLER (NVIC)***************************/
/* 0xE000E100 Base address */

#define NVIC_PERIPHERAL_BASE	0xE000E100

typedef struct{
	u32 ISER[128];
	u32 ICER[128];
	u32 ISPR[128];
	u32 ICPR[128];
	u32 IABR[128];
	u32 RESERVED[128];
	u8 IPR;
}NVIC_dtype;

#define NVIC		((volatile NVIC_dtype*)(NVIC_PERIPHERAL_BASE))


/******************************RCC*****************************************************/
#define RCC_CR			*((volatile u32*)(0x40023800 + 0x00))
#define RCC_PLLCFGR		*((volatile u32*)(0x40023800 + 0x04))
#define RCC_CFGR		*((volatile u32*)(0x40023800 + 0x08))
#define RCC_CIR			*((volatile u32*)(0x40023800 + 0x0C))
#define RCC_AHB1ENR		*((volatile u32*)(0x40023800 + 0x30))
#define RCC_AHB2ENR		*((volatile u32*)(0x40023800 + 0x34))
#define RCC_AHB3ENR		*((volatile u32*)(0x40023800 + 0x38))
#define RCC_APB1ENR		*((volatile u32*)(0x40023800 + 0x40))
#define RCC_APB2ENR		*((volatile u32*)(0x40023800 + 0x44))
#define RCC_AHB1LENR	*((volatile u32*)(0x40023800 + 0x50))
#define RCC_AHB2LENR	*((volatile u32*)(0x40023800 + 0x54))
#define RCC_APB1LENR	*((volatile u32*)(0x40023800 + 0x60))
#define RCC_APB2LENR	*((volatile u32*)(0x40023800 + 0x64))
#define RCC_BDCR		*((volatile u32*)(0x40023800 + 0x70))
#define RCC_CSR			*((volatile u32*)(0x40023800 + 0x74))
#define RCC_SSCGR		*((volatile u32*)(0x40023800 + 0x80))
#define RCC_PLLI2SCFGR	*((volatile u32*)(0x40023800 + 0x84))
#define RCC_DCKCFGR		*((volatile u32*)(0x40023800 + 0x8C))

/****************************UART***************************************************/
#define USART1_BASE_ADDRESS			0x40011000
#define USART2_BASE_ADDRESS			0x40004400
#define USART6_BASE_ADDRESS			0x40011400

#define USART1_SR		*((volatile u32*)(USART1_BASE_ADDRESS + 0x00))
#define USART1_DR		*((volatile u32*)(USART1_BASE_ADDRESS + 0x04))
#define USART1_BRR		*((volatile u32*)(USART1_BASE_ADDRESS + 0x08))
#define USART1_CR1		*((volatile u32*)(USART1_BASE_ADDRESS + 0x0C))
#define USART1_CR2		*((volatile u32*)(USART1_BASE_ADDRESS + 0x10))
#define USART1_CR3		*((volatile u32*)(USART1_BASE_ADDRESS + 0x14))
#define USART1_GTPR		*((volatile u32*)(USART1_BASE_ADDRESS + 0x18))

#define USART2_SR		*((volatile u32*)(USART2_BASE_ADDRESS + 0x00))
#define USART2_DR		*((volatile u32*)(USART2_BASE_ADDRESS + 0x04))
#define USART2_BRR		*((volatile u32*)(USART2_BASE_ADDRESS + 0x08))
#define USART2_CR1		*((volatile u32*)(USART2_BASE_ADDRESS + 0x0C))
#define USART2_CR2		*((volatile u32*)(USART2_BASE_ADDRESS + 0x10))
#define USART2_CR3		*((volatile u32*)(USART2_BASE_ADDRESS + 0x14))
#define USART2_GTPR		*((volatile u32*)(USART2_BASE_ADDRESS + 0x18))

#define USART6_SR		*((volatile u32*)(USART6_BASE_ADDRESS + 0x00))
#define USART6_DR		*((volatile u32*)(USART6_BASE_ADDRESS + 0x04))
#define USART6_BRR		*((volatile u32*)(USART6_BASE_ADDRESS + 0x08))
#define USART6_CR1		*((volatile u32*)(USART6_BASE_ADDRESS + 0x0C))
#define USART6_CR2		*((volatile u32*)(USART6_BASE_ADDRESS + 0x10))
#define USART6_CR3		*((volatile u32*)(USART6_BASE_ADDRESS + 0x14))
#define USART6_GTPR		*((volatile u32*)(USART6_BASE_ADDRESS + 0x18))

#define RCC_APB2ENR		*((volatile u32*)(0x40023800 + 0x44))
#define RCC_APB1ENR		*((volatile u32*)(0x40023800 + 0x40))


/*********************************EXTERNAL INTERRUPTS*******************************/
#define EXTI_BASE_ADDRESS		0x40013C00
#define SYSCFG_BASE_ADDRESS		0x40013800

#define EXTI_IMR		*((volatile u32* )(EXTI_BASE_ADDRESS + 0x00))
#define EXTI_EMR		*((volatile u32* )(EXTI_BASE_ADDRESS + 0x04))
#define EXTI_RTSR		*((volatile u32* )(EXTI_BASE_ADDRESS + 0x08))
#define EXTI_FTSR		*((volatile u32* )(EXTI_BASE_ADDRESS + 0x0C))
#define EXTI_SWIER		*((volatile u32* )(EXTI_BASE_ADDRESS + 0x10))
#define EXTI_PR			*((volatile u32* )(EXTI_BASE_ADDRESS + 0x14))


/******************************SYSCGF***********************************************/
typedef struct{
	u32 MEMRMP;
	u32 PMC;
	u32 EXTICR[4];
	u32 RESERVED[2];
	u32 CMPCR;
}SYSCFG_dtype;

#define SYSCFG	((volatile SYSCFG_dtype*)(SYSCFG_BASE_ADDRESS))

/******************************DMA**************************************************/
#define DMA1_BASE_ADDRESS		0x40026000
#define DMA2_BASE_ADDRESS		0x40026400

typedef struct{
	u32 CR;
	u32 NDTR;
	u32 PAR;
	u32 M0AR;
	u32 M1AR;
	u32 FCR;
}streamRegisters_dtype;

typedef struct{
	u16 ISR[4];
	u16 IFCR[4];
	streamRegisters_dtype S[8];
}DMA_dtype;

/*********************************SYSTICK******************************************/
typedef struct
{
  u32 CTRL;                    /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
  u32 LOAD;                    /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register       */
  u32 VALUE;                     /*!< Offset: 0x008 (R/W)  SysTick Current Value Register      */
  u32 CALIB;                   /*!< Offset: 0x00C (R/ )  SysTick Calibration Register        */
} SysTick_Type;

#define SCS_BASE            (0xE000E000UL) /*!< System Control Space Base Address  */
#define SysTick_BASE        (SCS_BASE +  0x0010UL)
#define SysTick             ((SysTick_Type   *)     SysTick_BASE  )   /*!< SysTick configuration struct       */

#define AHB_DIV_8		0
#define AHB				4

#define COUNTFLAG_BIT	16




#endif /* INC_PORT_H_ */
