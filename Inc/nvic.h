/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 2/1/2021				*************/
/*********** Version	: V01					*************/
/************************************************************/

/************************************************************/
/*			GUARD FILE TO PREVENT MULTIPLE INCLUSION		*/
/************************************************************/
#ifndef	_NVIC_INTERFACE_H_
#define	_NVIC_INTERFACE_H_


/************************************************************/
/*						FUNCTION PROTOTYPES					*/
/************************************************************/

void NVIC_voidEnableInterrupt(u8 copy_u8IntNumber);
void NVIC_voidDisableInterrupt(u8 copy_u8IntNumber);
void NVIC_voidSetPendingFlag(u8 copy_u8IntNumber);
void NVIC_voidResetPendingFlag(u8 copy_u8IntNumber);
u8 	 NVIC_voidGetActiveFlag(u8 copy_u8IntNumber);

#endif
