/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 1/6/2022				*************/
/*********** Version	: V02					*************/
/************************************************************/


/************************************************************/
/*							INCLUDES						*/
/************************************************************/
#include "stdtypes.h"
#include "macros.h"
#include "port.h"
#include <stdlib.h>

#include "bartos_config.h"
#include "bartos.h"
#include "bartostimer.h"





static bartosTimer_dtype* curr_timer = NULL;
static bartosTimer_dtype* head_timer = NULL;
bartosTimer_dtype timers[MAX_NUMBER_OF_TASKS] = {0};
u8 curr_timer_idx = 0;
static u32 curr_tick = 0;

/************************************************************/
/*						FUNCTION DEFINITIONS				*/
/************************************************************/

u8 ostimerInit(void){
	/* choose the clock and enable the peripheral's interrupt */
	SysTick->CTRL = CLOCK_SOURCE | 0x02;
	u32 timer_u32Ticks = (1000000U)/SYSTEM_TICKS_PER_SEC;
	/* set the load register */
	if(timer_u32Ticks < 0xFFFFFF)
	{
		SysTick->LOAD = timer_u32Ticks - 1;
	}

	/* start the count down */
	SysTick->CTRL |= 0x01;

	return OK;
}

u8 ostimerRegister(bartosTimer_dtype* timer_ptr){
	u8 status;
	if(head_timer == NULL){
		head_timer = timer_ptr;
		curr_timer = timer_ptr;
		curr_timer->next_timer = NULL;
		status = OK;
	}
	else{
		curr_timer->next_timer = timer_ptr;
		curr_timer = timer_ptr;
		curr_timer->next_timer = NULL;
		status = OK;
	}

	return status;
}

u8 ostimerCancel(bartosTimer_dtype* timer_ptr){
	u8 status = ERROR;
	if(head_timer == NULL){
		status = ERROR;
	}
	else if(head_timer->next_timer == NULL){
		if(head_timer == timer_ptr){
			head_timer = NULL;
			status = OK;
		}
		else{
			/* the timer does not exist in the linked-list of bartos timers */
			status = ERROR;
		}
	}
	else{
		if(head_timer == timer_ptr){
			head_timer = head_timer->next_timer;
			status = ERROR;
		}
		else{
			bartosTimer_dtype* prev_timer = head_timer;
			bartosTimer_dtype* curr_timer = head_timer->next_timer;
			while(curr_timer){
				if(curr_timer == timer_ptr){
					prev_timer->next_timer = curr_timer->next_timer;
					free(timer_ptr);
					status = OK;
				}
				else{
					prev_timer = curr_timer;
					curr_timer = curr_timer->next_timer;
				}
			}
		}
	}

	return status;
}

static void osDelayCallBack(void* tcb_void_ptr){
	/* if the ticks expire, add the tcb to the ready queue */
	tcb_dtype* tcb_ptr = (tcb_dtype*)tcb_void_ptr;
	osResumeTask(tcb_ptr);
}

static void ostimerManageCallBacks(){
	bartosTimer_dtype* curr_timer = head_timer;
	while(curr_timer != NULL){
		curr_timer->u32Ticks--;
		if(curr_timer->u32Ticks == 0){
			curr_timer->function_ptr(curr_timer->callback_args);
			ostimerCancel(curr_timer);
		}
		curr_timer = curr_timer->next_timer;
	}
}

void ostimerTick(void){		/* to be called inside the SysTick_Handler */
	if(osIsStarted()){
		curr_tick = curr_tick + 1;
		ostimerManageCallBacks();
	}
}

void BARTOS_delayTask(u32 u32Ticks){		/* to be called inside any task to suspend it until defined timer ticks pass */
	__asm("CPSID		I");		/* interrupts are enabled again at the end of context switching */
	tcb_dtype* curr_tcb_ptr;
	if(curr_timer_idx >= 10){
		curr_timer_idx = curr_timer_idx % 10;
	}
	else{
		/* Do nothing */
	}
	bartosTimer_dtype* newTimer_ptr = &timers[curr_timer_idx];
	curr_timer_idx++;
	curr_tcb_ptr = osGetCurrentTcb();
	curr_tcb_ptr->state = SUSPENDED;

	newTimer_ptr->function_ptr = osDelayCallBack;
	newTimer_ptr->callback_args = (void*)curr_tcb_ptr;
	newTimer_ptr->u32Ticks = u32Ticks;

	/* register the timer to this callback and this tcb_ptr */
	ostimerRegister(newTimer_ptr);
	/* manage the ready queue to see which task is running next and load it's context to switch immediately */
	osForceContextSwitching();
}
