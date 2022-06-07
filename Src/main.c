/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 13/3/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/
#include "main.h"



/***************************************Global variables*******************************************/
u8 system_state = GREEN;
u8 rfid_rcv_buffer[100];
msgQueueHandler_dtype rfid_rcv_queue;





/**************************************Forward declaration******************************************/
void setup_io_configs(void);
void setup_uart1_configs(void);
void setup_pwm_leds_configs(void);	/* helal */
void setup_adc_button_configs(void);	/* yehia */
void setup_seven_segment_io_configs(void);
u8 get_button_adc_read(void);			/* yehia */
void set_led1_on(void);		/* helal */
void set_led2_on(void);		/* helal */
void set_led3_on(void);		/* helal */
void set_led1_off(void);	/* helal */
void set_led2_off(void);	/* helal */
void set_led3_off(void);	/* helal */



/***************************************OS Tasks****************************************************/
void checkEmergencyTask(void){
	u8 rcvd_data;
	while(TRUE){
		if(BARTOS_QueueGet(rfid_rcv_queue, 10000, &rcvd_data) == OK){
			GPIO_TogglePin('G', P13);
		}
		else{
			/* didn't receive anything in the last 10 sec */
			GPIO_TogglePin('G', P14);
		}
	}
}





/****************************************Entry Point*******************************************/
int main(void) {
	RCC_initSystemClock();
	setup_io_configs();
	setup_uart1_configs();
	rfid_rcv_queue = BARTOS_createQueue(rfid_rcv_buffer, 100);
	BARTOS_createTask(checkEmergencyTask, 2);

	while (1) {

		/* start the rtos */
		BARTOS_start();
	}
	return 0;
}





/**************************************Application setup fucntions************************************/

void setup_io_configs(void){
	/* setup I/O configs */
	GPIO_Init('G', P13, OUTPUT, PUSH_PULL, NO_PULLING);
	GPIO_Init('G', P14, OUTPUT, PUSH_PULL, NO_PULLING);
}

void setup_uart1_configs(void){

	/* setup RX pin */
	GPIO_Init('A', P10, ALTERNATE_FUN, PUSH_PULL, NO_PULLING);
	GPIO_setAlternateFunction('A', P10, AF7);

	uartConfig_dtype Uart1_config;

	Uart1_config.parity.enable = 					DISABLE;
	Uart1_config.parity.type = 						EVEN;
	Uart1_config.over_eight = 						DISABLE;
	Uart1_config.clock_phase = 						FIRST_CLOCK;
	Uart1_config.clock_polarity = 					STEADY_LOW;
	Uart1_config.clock_synch = 						DISABLE;
	Uart1_config.parity_error_interrupt = 			DISABLE;
	Uart1_config.receive_complete_interrupt = 		ENABLE;
	Uart1_config.receiver =							ENABLE;
	Uart1_config.stopBit = 							HALF_STOP_BIT;
	Uart1_config.word_length = 						EIGHT;
	Uart1_config.transmission_complete_interrupt = 	DISABLE;
	Uart1_config.transmitter = 						DISABLE;
	Uart1_config.dma_receiver = 					DISABLE;
	Uart1_config.dma_transmitter = 					DISABLE;

	/* enable UART1 global interrupt from NVIC */
	NVIC_voidEnableInterrupt(_USART1_VECTOR_IRQ);

	/* initiate the USART1 */
	USART1_voidInit(&Uart1_config);
}

void USART1_IRQHandler(void){
	BARTOS_IntEnterRoutine();

	BARTOS_QueuePut(rfid_rcv_queue, -1, USART1_u8ReceiveCharacter());

	BARTOS_IntExitRoutine();
}


/*
 * \b setup_pwm_leds_configs
 *
 * This function should attach 3 leds to 3 pwm channels.
 *
 * all channels' frequency are 100HZ
 *
 * led is ON using 70% duty cycle
 *
 */
void setup_pwm_leds_configs(void){


}

/*
 * \b setup_adc_button_configs
 *
 * This function should attach a user button to an ADC channel
 */
void setup_adc_button_configs(void){


}


/*
 * \b get_button_adc_read
 *
 * This function should return ON when the ADC value read from the button is between 2 and 5 volts.
 * return OFF otherwise.
 *
 */
u8 get_button_adc_read(void){

}

/*
 * set_ledx_on
 *
 * This function should set the corresponding led to ON (70% dutycycle)
 *
 */
void set_led1_on(void){

}
void set_led2_on(void){

}
void set_led3_on(void){

}

/*
 * set_ledx_on
 *
 * This function should set the corresponding led to OFF (0% dutycycle)
 *
 */
void set_led1_off(void){

}
void set_led2_off(void){

}
void set_led3_off(void){

}
