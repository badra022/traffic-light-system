/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 13/3/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/
#include "main.h"

binarySemphrHandle_dtype semphr1 = NULL;

void toggleLed1(void){
	while(1){
		if(BARTOS_semaphoreGet(semphr1, 2000) == FALSE){
			/* do some thing else */
		}
		else{
			/* toggle led's state */
			GPIO_TogglePin('G', P13);
			BARTOS_delayTask(1000);
			GPIO_TogglePin('G', P13);
			BARTOS_semaphorePut(semphr1);
		}
	}
}

void toggleLed2(void){
	while(1){
		if(BARTOS_semaphoreGet(semphr1, 2000) == FALSE){
			/* do some thing else */
		}
		else{
			/* toggle led's state */
			GPIO_TogglePin('G', P14);
			BARTOS_delayTask(1000);
			GPIO_TogglePin('G', P13);
			BARTOS_semaphorePut(semphr1);
		}
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
	semphr1 = BARTOS_createBinarySemaphore();
	BARTOS_createTask(toggleLed1, 2);
	BARTOS_createTask(toggleLed2, 2);
	while (1) {

		/* start the rtos */
		BARTOS_start();
	}
	return 0;
}
