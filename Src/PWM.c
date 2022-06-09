/*
 ============================================================================
 Name        : PWM.c
 Author      : Ahmed Khaled Hilal
 Description : PWM driver source file
 ============================================================================
*/

#include "stdtypes.h"
#include "macros.h"
#include "port.h"

#include "gpt.h"
#include "PWM.h"

static u32 preload_value = 0;
/*duty cycle is from (0 - 100) and this means 10% , 20% and so on*/

void PWM_INIT(Gptim2_5_dtype* timer, u8 channel_no, u16 prescaler, u32 duty_cycle, u32 preload){

	preload_value = preload;

	if( (duty_cycle <= 100) && (duty_cycle >= 0)){
			/*pass it's ok*/
		}
		else{
			duty_cycle = 50;  	/*duty_cycle will be set to 50 as default*/
		}


	/*edge-align mode*/
	/*timer->CR1 &= ~(0x03 << 5);*/

	/* enable the auto-reload pre-load */
	timer->CR1 &= ~(0x01 << 7);
	timer->CR1 |= (0x01<<7);

	/*set direction of counting up-counting*/
	/*timer->CR1 &= ~(0x01 << 4);*/

	/*enable channel*/
	timer->CCER |= (1 << ((channel_no - 1)*4));

	/*SET ARR AND PRESCALER*/
	timer->ARR = preload_value;
	timer->PSC |=  prescaler - 1;


	switch(channel_no){

		case 1:
		{
			/*PWM mode 1 (110)*/
			/*timer->CCMR1 &= ~(0x06 << 4);*/
			timer->CCMR1 |= (0x06 << 4);

			/*pre-load enable*/
			timer->CCMR1 |= (0x01 << 3);

			/*set duty cycle*/
			timer-> CCR1 = (preload * duty_cycle) / 100;

		}
		break;
		case 2:
		{
			/*PWM mode 1 (110)*/
			timer->CCMR1 &= ~(0x06 << 12);
			timer->CCMR1 |= (0x06 << 12);

			/*pre-load enable*/
			timer->CCMR1 |= (0x01 << 11);

			/*set duty cycle*/
			timer-> CCR2 = (preload * duty_cycle) / 100;
		}
		break;
		case 3:
		{
			/*PWM mode 1 (110)*/
			timer->CCMR2 &= ~(0x06 << 4);
			timer->CCMR2 |= (0x06 << 4);

			/*pre-load enable*/
			timer->CCMR2 |= (0x01 << 3);

			/*set duty cycle*/
			timer-> CCR3 = (preload * duty_cycle) / 100;
		}
		break;
		case 4:
		{
			/*PWM mode 1 (110)*/
			timer->CCMR2 &= ~(0x06 << 12);
			timer->CCMR2 |= (0x06 << 12);

			/*pre-load enable*/
			timer->CCMR2 |= (0x01 << 11);

			/*set duty cycle*/
			timer-> CCR4 = (preload * duty_cycle) / 100;
		}
		break;

	}





}


void PWM_START_TIMER(Gptim2_5_dtype* timer){
	timer->CR1 |= (0x01 << 0);
}


static void PWM_STOP_TIMER(Gptim2_5_dtype* timer){
	timer->CR1 &= ~(0x01 << 0);
}


void PWM_ChangeDutycycle(Gptim2_5_dtype* timer, u8 channel_no, u8 duty_cycle){

	PWM_STOP_TIMER(timer);
	volatile u32* channels_CCR[4] = {&(timer-> CCR1),&(timer-> CCR2),&(timer-> CCR3),&(timer-> CCR4)};

	if( (duty_cycle <= 100) && (duty_cycle >= 0)){
		*channels_CCR[channel_no - 1] = (preload_value * duty_cycle) / 100;
	}
	else{
		/*pass leave duty cycle as it is*/
	}
	PWM_START_TIMER(timer);
}
