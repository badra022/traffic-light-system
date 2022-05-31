/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 13/3/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/
#include "main.h"

void portable_delay_cycles(unsigned long n)
{
 	while (n--)
 	{
 		asm volatile ("");
 	}
}

void toggleLed1(void){
	while(1){
	/* toggle led's state */
	GPIO_TogglePin('G', P13);
	BartosTimer_Delay(1000);
	}
}

void toggleLed2(void){
	while(1){
	/* toggle led's state */
	GPIO_TogglePin('G', P14);
	BartosTimer_Delay(1000);
	}
}

void setupIOConfigs(void){
	/* setup I/O configs */
	GPIO_Init('G', P13, OUTPUT, PUSH_PULL, NO_PULLING);
	GPIO_Init('G', P14, OUTPUT, PUSH_PULL, NO_PULLING);
}

int main(void) {
	setupIOConfigs();
	GPIO_WritePin('G', P13, HIGH);
	GPIO_WritePin('G', P14, HIGH);
	Bartos_createTask(toggleLed1, 1);
	Bartos_createTask(toggleLed2, 2);
	while (1) {

		/* start the rtos */
		Bartos_start();
	}
	return 0;
}
