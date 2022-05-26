/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 13/3/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/
#ifndef INC_GPT_H_
#define INC_GPT_H_

#define _TIM2_VECTOR_IRQ		28
#define _TIM3_VECTOR_IRQ		29
#define _TIM4_VECTOR_IRQ		30
#define _TIM5_VECTOR_IRQ		50

#define _TIM2_RCC_ID			0
#define _TIM3_RCC_ID			1
#define _TIM4_RCC_ID			2
#define _TIM5_RCC_ID			3

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

typedef enum{
	UPDATE_INTERRUPT = 0,
	TRIGGER_INTERRUPT = 6,
}timerFlags_dtype;

#define TIM2	((Gptim2_5_dtype*)(0x40000000))
#define TIM3	((Gptim2_5_dtype*)(0x40000400))
#define TIM4	((Gptim2_5_dtype*)(0x40000800))
#define TIM5	((Gptim2_5_dtype*)(0x40000C00))

#define UPCOUNTING		0
#define DOWNCOUNTING	1

#define UPDATE_GENERATION	(1 << 0)
#define TRIGGER_GENERATION	(1 << 6)

void TIM_voidInit(Gptim2_5_dtype* timer, u8 counting_direction, u32 initial_cnt_value);
void TIM_voidStartTimer(Gptim2_5_dtype* timer);
void TIM_voidSoftwareInterrupt(Gptim2_5_dtype* timer, timerFlags_dtype flag);
void TIM_voidClearInterruptFlag(Gptim2_5_dtype* timer, timerFlags_dtype flag);
void TIM_voidSetCallBackFunction(Gptim2_5_dtype* timer, void (*function_address)(void));
u8	 TIM_u8GetFlagStatus(Gptim2_5_dtype* timer, timerFlags_dtype flag);

#endif /* INC_GPT_H_ */
