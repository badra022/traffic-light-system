/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 30/8/2020				*************/
/*********** Version	: V02					*************/
/************************************************************/


/************************************************************/
/*							INCLUDES						*/
/************************************************************/
#include"MATH_macros.h"
#include "stdtypes.h"
#include <stdlib.h>

#include "systick.h"
#include "systick_private.h"
#include "bartos.h"

/* options: AHB_DIV_8, AHB */
#define CLOCK_SOURCE				AHB_DIV_8

typedef struct bartos_timer{
	FUNCTION_PTR_VOID_ARGS		function_ptr;
	u32					u32Ticks;
	void*				callback_args;
	struct bartos_timer* next_timer;
}bartosTimer_dtype;

static bartosTimer_dtype* curr_timer = NULL;
static bartosTimer_dtype* head_timer = NULL;
static u32 curr_tick = 0;



/************************************************************/
/*						FUNCTION DEFINITIONS				*/
/************************************************************/

ErrorStatus BartosTimer_Init(void){
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

	return SUCCESS;
}

static ErrorStatus BartosTimer_Register(bartosTimer_dtype* timer_ptr){
	ErrorStatus state;
	if(head_timer == NULL){
		head_timer = timer_ptr;
		curr_timer = timer_ptr;
		state = SUCCESS;
	}
	else{
		curr_timer->next_timer = timer_ptr;
		curr_timer = timer_ptr;
		curr_timer->next_timer = NULL;
		state = SUCCESS;
	}

	return state;
}

static ErrorStatus BartosTimer_Cancel(bartosTimer_dtype* timer_ptr){
	ErrorStatus state = ERROR;
	if(head_timer == NULL){
		state = ERROR;
	}
	else if(head_timer->next_timer == NULL){
		if(head_timer == timer_ptr){
			head_timer = NULL;
			free(timer_ptr);
			state = SUCCESS;
		}
		else{
			/* the timer does not exist in the linked-list of bartos timers */
			state = ERROR;
		}
	}
	else{
		if(head_timer == timer_ptr){
			head_timer = head_timer->next_timer;
			state = ERROR;
		}
		else{
			bartosTimer_dtype* prev_timer = head_timer;
			bartosTimer_dtype* curr_timer = head_timer->next_timer;
			while(curr_timer){
				if(curr_timer == timer_ptr){
					prev_timer->next_timer = curr_timer->next_timer;
					free(timer_ptr);
					state = SUCCESS;
				}
				else{
					prev_timer = curr_timer;
					curr_timer = curr_timer->next_timer;
				}
			}
		}
	}

	return state;
}

static void delayCallBack(void* tcb_void_ptr){
	/* if the ticks expire, add the tcb to the ready queue */
	tcb_dtype* tcb_ptr = (tcb_dtype*)tcb_void_ptr;
	Bartos_resumeTask(tcb_ptr);
}

static void BartosTimer_ManageCallBacks(){
	bartosTimer_dtype* curr_timer = head_timer;
	while(curr_timer != NULL){
		curr_timer->u32Ticks--;
		if(curr_timer->u32Ticks == 0){
			curr_timer->function_ptr(curr_timer->callback_args);
			BartosTimer_Cancel(curr_timer);
		}
		curr_timer = curr_timer->next_timer;
	}
}

void BartosTimer_TimerTick(void){		/* to be called inside the SysTick_Handler */
	if(Bartos_IsStarted()){
		curr_tick = curr_tick + 1;
		BartosTimer_ManageCallBacks();
	}
}

void BartosTimer_Delay(u32 u32Ticks){		/* to be called inside any task to suspend it until defined timer ticks pass */
	tcb_dtype* curr_tcb_ptr;
	bartosTimer_dtype* newTimer_ptr = (bartosTimer_dtype*)malloc(sizeof(bartosTimer_dtype));

	curr_tcb_ptr = Bartos_getCurrentTcb();
	curr_tcb_ptr->state = SUSPENDED;

	newTimer_ptr->function_ptr = delayCallBack;
	newTimer_ptr->callback_args = (void*)curr_tcb_ptr;
	newTimer_ptr->u32Ticks = u32Ticks;

	/* remove the tcb from ready queue */
	Bartos_dequeueTcbEntry(&TcbPtrQueueHead, curr_tcb_ptr);
	/* register the timer to this callback and this tcb_ptr */
	BartosTimer_Register(newTimer_ptr);
}

u32  BartosTimer_u32GetRemainingTicks(void){
	return (SYSTEM_TICKS_PER_SEC - curr_tick);
}

u32  BartosTimer_u32GetElapsedTicks(void){
	return curr_tick;
}

