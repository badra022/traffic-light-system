/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 13/3/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/
#include "main.h"

#define GREEN		0
#define EMERGENCY	20
#define YELLOW		1
#define RED			2

u8 system_state = GREEN;
u8 rfid_rcv_buffer[100];
msgQueueHandler_dtype rfid_rcv_queue;

void USART1_IRQHandler(void){
	BARTOS_IntEnterRoutine();

	BARTOS_QueuePut(rfid_rcv_queue, -1, USART1_u8ReceiveCharacter());

	BARTOS_IntExitRoutine();
}

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

int main(void) {
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
