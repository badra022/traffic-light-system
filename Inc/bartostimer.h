/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 28/5/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/


/************************************************************/
/*			GUARD FILE TO PREVENT MULTIPLE INCLUSION		*/
/************************************************************/
#ifndef _BARTOS_TIMER_H_
#define _BARTOS_TIMER_H_

typedef struct bartos_timer{
	FUNCTION_PTR_VOID_ARGS		function_ptr;
	u32					u32Ticks;
	void*				callback_args;
	struct bartos_timer* next_timer;
}bartosTimer_dtype;

extern bartosTimer_dtype timers[];
extern u8 curr_timer_idx;
/************************************************************/
/*						FUNCTION PROTOTYPES					*/
/************************************************************/
u8 ostimerInit(void);
void BARTOS_delayTask(u32 u32Ticks);
void ostimerTick(void);
u8 ostimerCancel(bartosTimer_dtype* timer_ptr);
u8 ostimerRegister(bartosTimer_dtype* timer_ptr);

#endif	/* _BARTOS_TIMER_H_ */
