/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 30/8/2020				*************/
/*********** Version	: V02					*************/
/************************************************************/


/************************************************************/
/*			GUARD FILE TO PREVENT MULTIPLE INCLUSION		*/
/************************************************************/
#ifndef _SYSTICK_INTERFACE_H_
#define _SYSTICK_INTERFACE_H_



/************************************************************/
/*						FUNCTION PROTOTYPES					*/
/************************************************************/
void STK_voidInit(void);
void STK_voidDelay(u32 u32Ticks);
void STK_SetPeriodicFunction(u32 u32Ticks, FUNCTION_PTR function);
void STK_SetPeriodicTicks(u32 u32Ticks);
void STK_voidStopTimer(void);
u32  STK_u32GetRemainingTime(void);
u32  STK_u32GetElapsedTime(void);

#endif
