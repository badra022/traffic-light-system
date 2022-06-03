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



typedef enum{
	UPDATE_INTERRUPT = 0,
	TRIGGER_INTERRUPT = 6,
}timerFlags_dtype;

#define TIM2	((void*)(TIM2_BASE_ADDRESS))
#define TIM3	((void*)(TIM3_BASE_ADDRESS))
#define TIM4	((void*)(TIM4_BASE_ADDRESS))
#define TIM5	((void*)(TIM5_BASE_ADDRESS))

#define UPCOUNTING		0
#define DOWNCOUNTING	1

#define UPDATE_GENERATION	(1 << 0)
#define TRIGGER_GENERATION	(1 << 6)

void TIM_voidInit(void* timer, u8 counting_direction, u32 initial_cnt_value);
void TIM_voidStartTimer(void* timer);
void TIM_voidSoftwareInterrupt(void* timer, timerFlags_dtype flag);
void TIM_voidClearInterruptFlag(void* timer, timerFlags_dtype flag);
void TIM_voidSetCallBackFunction(void* timer, void (*function_address)(void));
u8	 TIM_u8GetFlagStatus(void* timer, timerFlags_dtype flag);

#endif /* INC_GPT_H_ */
