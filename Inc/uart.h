/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 4/1/2021				*************/
/*********** Version	: V01					*************/
/************************************************************/

/************************************************************/
/*			GUARD FILE TO PREVENT MULTIPLE INCLUSION		*/
/************************************************************/
#ifndef INC_UART_H_
#define INC_UART_H_

#define _USART1_VECTOR_IRQ		37
#define _USART2_VECTOR_IRQ		38

typedef enum{
	EIGHT,
	NINE,
}wordLenght_dtype;

typedef enum{
	EVEN,
	ODD,
}paritySelection_dtype;

typedef struct{
	u8 enable;						/*	ENABLE, DISABLE */
	paritySelection_dtype type;		/* EVEN, ODD */
}parity_dtype;

typedef enum{
	ONE_STOP_BIT,
	HALF_STOP_BIT,
	TWO_STOP_BIT,
	ONE_AND_HALF_STOP_BIT,
}stopBit_dtype;

typedef enum{
	STEADY_LOW,
	STEADY_HIGH,
}clockPolarity_dtype;

typedef enum{
	FIRST_CLOCK,
	SECOND_CLOCK,
}clockPhase_dtype;

typedef struct{
	parity_dtype parity;			/* (ENABLE, ODD), (ENABLE, EVEN), (DISABLE, xx) */
	u8 over_eight;					/*  ENABLE, DISABLE */
	wordLenght_dtype word_length;	/*  EIGHT , NINE */
	u8 transmission_complete_interrupt;		/* ENABLE, DISABLE */
	u8 receive_complete_interrupt;			/* ENABLE, DISABLE */
	u8 parity_error_interrupt;				/* ENABLE, DISABLE */
	u8 transmitter;							/* ENABLE, DISABLE */
	u8 receiver;							/* ENABLE, DISABLE */
	u8 clock_synch;							/* ENABLE, DISABLE */
	stopBit_dtype stopBit;					/* ONE_STOP_BIT */
	clockPolarity_dtype clock_polarity;		/* STEADY_LOW, STEADY_HIGH */
	clockPhase_dtype clock_phase;			/* FIRST_CLOCK, SECOND_CLOCK */
	u8 dma_transmitter;						/* ENABLE, DISABLE */
	u8 dma_receiver;						/* ENABLE, DISABLE */
}uartConfig_dtype;

/************************************************************/
/*						FUNCTION PROTOTYPES					*/
/************************************************************/
void USART1_voidInit(uartConfig_dtype* config_ptr);
void USART1_voidTransmitCharacter(u8 u8data);
u8 USART1_u8ReceiveCharacter(void);
void USART1_voidSendString(u8* str);
u32 USART1_u32DataRegisterAddress(void);

void USART2_voidInit(uartConfig_dtype* config_ptr);
void USART2_voidTransmitCharacter(u8 u8data);
u8 USART2_u8ReceiveCharacter(void);
void USART2_voidSendString(u8* str);
u32 USART2_u32DataRegisterAddress(void);

#endif /* INC_UART_H_ */
