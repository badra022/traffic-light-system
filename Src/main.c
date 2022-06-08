/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 13/3/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/
#include "main.h"



/***************************************Global variables*******************************************/
u8 emergency = FALSE;
u8 pedestrian_request = FALSE;
u8 rfid_rcv_buffer[100];
msgQueueHandler_dtype rfid_rcv_queue;
u8 svn_segment_1_number = 0;
u8 svn_segment_2_number = 1;
u8 pedestrain_button_poller_enable = TRUE;




/**************************************Forward declaration******************************************/
void setup_io_configs(void);
void setup_uart1_configs(void);
void setup_pwm_leds_configs(void);
void setup_adc_button_configs(void);	/* yehia */
void setup_seven_segment_io_configs(void);
u8 get_button_adc_read(void);			/* yehia */
void set_led1_on(void);
void set_led2_on(void);
void set_led3_on(void);
void set_led1_off(void);
void set_led2_off(void);
void set_led3_off(void);
void manageSystemStates(void);
void postPedestrianStateTask(void);
void pedestrianStateTask(void);
void defaultStateTask(void);

/***************************************OS Tasks****************************************************/
void checkEmergencyTask(void){			// 2
	u8 rcvd_data;
	while(TRUE){
		if(BARTOS_QueueGet(rfid_rcv_queue, 10000, &rcvd_data) == OK){
			emergency = TRUE;
		}
		else{
			/* didn't receive anything in the last 10 sec */
			/* or didn't receive the emergency car type rfid frame */
			emergency = FALSE;
		}
	}
}

void checkPedestrianRequestTask(void){		// 2
	static u8 eventCount = 0;
	while(TRUE){
		if(pedestrain_button_poller_enable == TRUE){
			if(get_button_adc_read() == TRUE){
				eventCount++;
				if(eventCount == 10){
					pedestrian_request = TRUE;
					eventCount = 0;
				}
				else{
					/* Still didn't reach 10 consecutive presses */
				}
			}
			else{
				eventCount = 0;
			}
		}
		else{
			/* polling is disabled, another pedestrain request is being handled */
			eventCount = 0;
		}
		BARTOS_delayTask(10);		/* 10 ms */
	}
}

void sevenSegmentDisplayTask(void){			// 3
	while(TRUE){
		svn_segment_1_enable();
		svn_segment_2_disable();
		svn_segment_write(svn_segment_1_number);
		BARTOS_delayTask(10);		/* 10 ms */
		svn_segment_1_disable();
		svn_segment_2_enable();
		svn_segment_write(svn_segment_2_number);
		BARTOS_delayTask(10);
	}
}

void pedestrianStateTask(void){		// 5
	u8 remaining_seconds = 10;
	set_led1_off();
	set_led2_on();		/* yellow */
	set_led3_off();
	while(remaining_seconds){
		svn_segment_1_number = remaining_seconds % 10;
		svn_segment_2_number = remaining_seconds / 10;
		remaining_seconds = remaining_seconds - 1;
		BARTOS_delayTask(1000);		/* 1 second */
	}

	remaining_seconds = 30;
	set_led1_off();
	set_led2_on();
	set_led3_off();		/* red */
	while(remaining_seconds){
		svn_segment_1_number = remaining_seconds % 10;
		svn_segment_2_number = remaining_seconds / 10;
		remaining_seconds = remaining_seconds - 1;
		BARTOS_delayTask(1000);		/* 1 second */
	}
	CRITICAL_SECTION_START();
	manageSystemStates();
	BARTOS_createTask(postPedestrianStateTask, 5);
	BARTOS_endTask();
}

void postPedestrianStateTask(void){		// 5
	BARTOS_delayTask(10000);		/* 10 seconds */
	if(pedestrian_request == TRUE){
		emergency = FALSE;
	}
	else{
		/* no pedestrain request happpened in past 10 seconds */
	}
	BARTOS_endTask();
}

void defaultStateTask(void){		// 5
	while(1){
		if(!(emergency == FALSE && pedestrian_request == TRUE)){		/* Exit condition */
			/* activate or refresh Default state */
			svn_segment_1_number = 0;
			svn_segment_2_number = 0;
			set_led1_on();		/* green */
			set_led2_off();
			set_led3_off();
			BARTOS_delayTask(100);		/* 100 ms */
		}
		else{
			/* Default state is not applicable */
			CRITICAL_SECTION_START();
			manageSystemStates();
			BARTOS_endTask();
		}
	}
}


/****************************************Entry Point*******************************************/
int main(void) {
	RCC_initSystemClock();
	setup_io_configs();
	setup_uart1_configs();
	setup_seven_segment_io_configs();
	rfid_rcv_queue = BARTOS_createQueue(rfid_rcv_buffer, 100);
	BARTOS_createTask(checkEmergencyTask, 2);
	BARTOS_createTask(checkPedestrianRequestTask, 2);
	BARTOS_createTask(sevenSegmentDisplayTask, 3);

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
	static u8 rfid_frame[3] = {0};
	static u8 char_idx = 0;

	rfid_frame[char_idx] = USART1_u8ReceiveCharacter();
	char_idx++;
	if(char_idx >= 3){
		char_idx = 0;
		if(rfid_frame[0] == 0xaa && rfid_frame[2] == 0x55){
			if(rfid_frame[1] == 0x02 || rfid_frame[1] == 0x03){
				BARTOS_QueuePut(rfid_rcv_queue, -1, rfid_frame[1]);
			}
			else{
				/* Car type is not emergency */
			}
		}
		else{
			/* Frame received is not an RFID frame as the Header and Tail does not match */
		}
	}
	else{
		/* Frame is not fully received */
	}
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

	RCC_voidEnableClock(RCC_APB1, _TIM5_RCC_ID);
	PWM_INIT(TIM5, 3, 1, 0, RCC_u32GetSystemClock() / 100);		// 100HZ pwm output on TIM5_CH1 with preset duty cycle
	PWM_INIT(TIM5, 3, 2, 0, RCC_u32GetSystemClock() / 100);		// 100HZ pwm output on TIM5_CH2 with preset duty cycle
	PWM_INIT(TIM5, 3, 3, 0, RCC_u32GetSystemClock() / 100);		// 100HZ pwm output on TIM5_CH3 with preset duty cycle
	GPIO_Init('A', P0, ALTERNATE_FUN, PUSH_PULL, NO_PULLING);
	GPIO_Init('A', P1, ALTERNATE_FUN, PUSH_PULL, NO_PULLING);
	GPIO_Init('A', P2, ALTERNATE_FUN, PUSH_PULL, NO_PULLING);
	GPIO_setAlternateFunction('A', P0, AF2);
	GPIO_setAlternateFunction('A', P1, AF2);
	GPIO_setAlternateFunction('A', P2, AF2);
	PWM_START_TIMER(TIM5);				// start pwm
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
	PWM_ChangeDutycycle(TIM0, 1, 70);
}
void set_led2_on(void){
	PWM_ChangeDutycycle(TIM0, 1, 70);
}
void set_led3_on(void){
	PWM_ChangeDutycycle(TIM0, 1, 70);
}

/*
 * set_ledx_off
 *
 * This function should set the corresponding led to OFF (0% dutycycle)
 *
 */
void set_led1_off(void){
	PWM_ChangeDutycycle(TIM0, 1, 0);
}
void set_led2_off(void){
	PWM_ChangeDutycycle(TIM0, 1, 0);
}
void set_led3_off(void){
	PWM_ChangeDutycycle(TIM0, 1, 0);
}

void setup_seven_segment_io_configs(void){
	svn_segment_init((u8)'B');
	svn_segment_write(0);
}

void manageSystemStates(void){
	if(emergency == TRUE && pedestrian_request == TRUE){

		BARTOS_createTask(defaultStateTask, 5);
	}
	else if(emergency == FALSE && pedestrian_request == TRUE){
		/* pedestrian request will be handled */

		pedestrain_button_poller_enable = FALSE;
		pedestrian_request = FALSE;
		BARTOS_createTask(pedestrianStateTask, 5);
	}
	else{
		pedestrain_button_poller_enable = TRUE;
		BARTOS_createTask(defaultStateTask, 5);
	}
}
