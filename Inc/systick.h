/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 28/5/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/


/************************************************************/
/*			GUARD FILE TO PREVENT MULTIPLE INCLUSION		*/
/************************************************************/
#ifndef _SYSTICK_INTERFACE_H_
#define _SYSTICK_INTERFACE_H_



#define SYSTEM_TICKS_PER_SEC		1000
/************************************************************/
/*						FUNCTION PROTOTYPES					*/
/************************************************************/
ErrorStatus BartosTimer_Init(void);
void BartosTimer_Delay(u32 u32Ticks);
u32  BartosTimer_u32GetRemainingTicks(void);
u32  BartosTimer_u32GetElapsedTicks(void);
void BartosTimer_TimerTick(void);

#endif
