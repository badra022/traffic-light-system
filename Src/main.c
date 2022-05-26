/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 13/3/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/
#include "main.h"

void APP_toggleLedTaskSetup(void){
	/* enable TIM clock */
	RCC_voidEnableClock(RCC_APB1, _TIM2_RCC_ID);
	/* setup the TIM2 configs */
	TIM_voidInit(TIM2, DOWNCOUNTING, RCC_u32GetSystemClock());
	/* enable TIM2 NVIC interrupt */
	NVIC_voidEnableInterrupt(_TIM2_VECTOR_IRQ);
	/* set the callback function that will be called inside the interrupt */
	TIM_voidSetCallBackFunction(TIM2, APP_toggleLed);
	/* start TIM2 */
	TIM_voidStartTimer(TIM2);
}

void APP_toggleLed(void){
	/* toggle led's state */
	GPIO_TogglePin('G', P13);
}

void APP_setupIOConfigs(void){
	/* setup I/O configs */
	GPIO_Init('G', P13, OUTPUT, PUSH_PULL, NO_PULLING);
}


int main(void) {
	/* setup I/O configs */
	APP_setupIOConfigs();
	/* setup tim2 */
	APP_toggleLedTaskSetup();

	while (1) {
		/* Infinite loop */
	}
	return 0;
}
