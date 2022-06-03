/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 2/1/2021				*************/
/*********** Version	: V01					*************/
/************************************************************/

#ifndef INC_EXTI_H_
#define INC_EXTI_H_

#define _EXTI0_VECTOR_IRQ		6
#define _EXTI1_VECTOR_IRQ		7
#define _EXTI2_VECTOR_IRQ		8
#define _EXTI3_VECTOR_IRQ		9
#define _EXTI4_VECTOR_IRQ		10

typedef enum{
	RISING_EDGE,
	FALLING_EDGE,
	BOTH,
}triggerState_dtype;

typedef enum{
	PA = 0,
	PB = 1,
	PC = 2,
	PD = 3,
	PE = 4,
	PH = 7,
}channelPort_dtype;

void EXTI_enableInterruptChannel(u32 copy_u32channelNumber, triggerState_dtype trigger, channelPort_dtype port);
void EXTI_disableInterruptChannel(u32 copy_u32channelNumber);
void EXTI_SoftwareInterrupt(u32 copy_u32channelNumber);
u8 EXTI_getPendingBit(u32 copy_u32channelNumber);

#endif /* INC_EXTI_H_ */
