/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 13/3/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/
#include "stdtypes.h"
#include "macros.h"
#include "port.h"

#include "gpt.h"


volatile static void (*tim2callbackfunction)(void) = NULL;
volatile static void (*tim3callbackfunction)(void) = NULL;
volatile static void (*tim4callbackfunction)(void) = NULL;
volatile static void (*tim5callbackfunction)(void) = NULL;
/*
 * assumptions:
 * 		default settings for the peripheral are
 *	 		edge aligned mode
 *			one pulse mode is off
 *			no auto reload preload
 *			update event is enabled and generate interrupt
 *			system clock is not set (default is HSI = 16 MHZ)
 *			No prescaler for the CLK_INT provided for the timer (thus, counter runs on 16 MHZ)
 *
 *
 */
void TIM_voidInit(void* timer, u8 counting_direction, u32 initial_cnt_value){
	Gptim2_5_dtype* timer_ptr = (Gptim2_5_dtype*)timer;
	/* enable the Update event generation */
	CLR_BIT(timer_ptr->CR1, 1);
	/* restrict the Update event generation to counter overflow/underflow events */
	SET_BIT(timer_ptr->CR1, 2);
	/* enable the auto-reload preload (buffering) */
	SET_BIT(timer_ptr->CR1, 7);
	/* set the counting direction */
	SET_VALUE(timer_ptr->CR1, 4, counting_direction);
	/* enable the Update event interrupt request */
	SET_BIT(timer_ptr->DIER, 0);

	/* init the counting register with the initial value */
	timer_ptr->CNT = initial_cnt_value;
	/* set the Auto-reload value to the same value */
	timer_ptr->ARR = initial_cnt_value;
}

void TIM_voidStartTimer(void* timer){
	Gptim2_5_dtype* timer_ptr = (Gptim2_5_dtype*)timer;
	SET_BIT(timer_ptr->CR1, 0);
}

void TIM_voidSoftwareInterrupt(void* timer, timerFlags_dtype flag){
	Gptim2_5_dtype* timer_ptr = (Gptim2_5_dtype*)timer;
	/* activate the selected interrupt request by software */
	timer_ptr->EGR |= (1 << flag);
}

void TIM_voidClearInterruptFlag(void* timer, timerFlags_dtype flag){
	Gptim2_5_dtype* timer_ptr = (Gptim2_5_dtype*)timer;
	/* clear the Update interrupt flag */
	CLR_BIT(timer_ptr->SR, flag);
}

u8	 TIM_u8GetFlagStatus(void* timer, timerFlags_dtype flag){
	Gptim2_5_dtype* timer_ptr = (Gptim2_5_dtype*)timer;
	return timer_ptr->SR & (1 << flag);
}


void TIM_voidSetCallBackFunction(void* timer, void (*function_address)(void)){
	/* assign the function address to the callback function pointer specific for selected timer */
	u32 timer_id = (timer - TIM2) / 0x400;
	switch(timer_id){
	case 0:
		tim2callbackfunction = function_address;
		break;
	case 1:
		tim3callbackfunction = function_address;
		break;
	case 2:
		tim4callbackfunction = function_address;
		break;
	case 3:
		tim5callbackfunction = function_address;
		break;
	}
}

/******************************************************************************/
/**************************INTERRUPT SERVICE ROUTINES**************************/
/******************************************************************************/

void TIM2_IRQHandler(void){
	/* clear the flags for the future interrupts */
	TIM_voidClearInterruptFlag(TIM2, UPDATE_INTERRUPT);
	/* execute the callback function */
	tim2callbackfunction();
}

void TIM3_IRQHandler(void){
	/* clear the flags for the future interrupts */
	TIM_voidClearInterruptFlag(TIM3, UPDATE_INTERRUPT);
	/* execute the callback function */
	tim3callbackfunction();
}

void TIM4_IRQHandler(void){
	/* clear the flags for the future interrupts */
	TIM_voidClearInterruptFlag(TIM4, UPDATE_INTERRUPT);
	/* execute the callback function */
	tim4callbackfunction();
}

void TIM5_IRQHandler(void){
	/* clear the flags for the future interrupts */
	TIM_voidClearInterruptFlag(TIM5, UPDATE_INTERRUPT);
	/* execute the callback function */
	tim5callbackfunction();
}
