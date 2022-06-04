/*
 * smphr.c
 *
 *  Created on: Jun 3, 2022
 *      Author: Badra
 */


#include "stdtypes.h"
#include "macros.h"
#include "port.h"
#include <stdlib.h>

#include "bartos_config.h"
#include "bartos.h"
#include "bartostimer.h"
#include "binarysemphr.h"


/* forward Declaration */
static void osBinarySemphrBlockTask(binarySemphrHandle_dtype semphr_handle, u32 ticks);



binarySemphrHandle_dtype BARTOS_createBinarySemaphore(void){
	binarySemphrHandle_dtype handle = (binarySemphrHandle_dtype)malloc(sizeof(binarySemphr_dtype));
	if(handle == NULL){
		/* failed to create the semaphore in heap */
	}
	else{
		handle->state = TRUE;
		handle->tcbBlockQueue = NULL;
	}
	return handle;
}


u8 BARTOS_semaphoreGet(binarySemphrHandle_dtype handle, u32 timeout){
	CRITICAL_SECTION_START();
	tcb_dtype* curr_tcb_ptr = osGetCurrentTcb();
	if(handle == NULL){
		/*   */
	}
	else{
		if(handle->state == TRUE){
			handle->state = FALSE;
			return TRUE;
		}
		else{
			if(timeout == (u32)-1){
				/* don't block the task and return FALSE. */
				return FALSE;
			}
			else if(timeout == 0){
				/* add the task to suspend queue in the semaphore. */
				curr_tcb_ptr->state = BLOCKED;
				osEnqueueTcbPriority(&(handle->tcbBlockQueue), curr_tcb_ptr);
				osForceContextSwitching();
			}
			else if(timeout > 0 && timeout <= (15 * SYSTEM_TICKS_PER_SEC)){
				/* add the task to suspend queue in the semaphore and attach timeout callback. */
				osBinarySemphrBlockTask(handle, timeout);
			}
		}
	}
	CRITICAL_SECTION_END();
	return curr_tcb_ptr->semphr_get_status;
}


u8 BARTOS_semaphorePut(binarySemphrHandle_dtype handle){

	CRITICAL_SECTION_START();
	u8 status;
	if(handle == NULL){
		status = ERR_INVALID_PARAMETER;
	}
	else if(handle->state == TRUE){
		status = ERR_INVALID_PARAMETER;
	}
	else{
		/* make state = TRUE and get the next task ready from the suspend Queue */
		tcb_dtype* next_tcb_ptr = osDequeueTcbHead(&(handle->tcbBlockQueue));
		if(next_tcb_ptr == NULL){
			handle->state = TRUE;
		}
		else{
			next_tcb_ptr->semphr_get_status = TRUE;
			next_tcb_ptr->blocking_semphr_handle = NULL;
			if(next_tcb_ptr->timer_handler != NULL){
				ostimerCancel((bartosTimer_dtype*)(next_tcb_ptr->timer_handler));
			}
			else{

			}

			osResumeTask(next_tcb_ptr);
			osForceContextSwitching();
			status = OK;
		}
	}

	CRITICAL_SECTION_END();
	return status;
}

static void osBlockCallBack(void* tcb_void_ptr){
	/* if the ticks expire, add the tcb to the ready queue */
	/* remove the tcb from the blocking queue of the suspending semaphore */
	tcb_dtype* tcb_ptr = (tcb_dtype*)tcb_void_ptr;
	binarySemphrHandle_dtype semphr_handle = (binarySemphrHandle_dtype)(tcb_ptr->blocking_semphr_handle);
	osDequeueTcbEntry(&(semphr_handle->tcbBlockQueue), tcb_ptr);
	tcb_ptr->semphr_get_status = FALSE;
	tcb_ptr->blocking_semphr_handle = NULL;
	osResumeTask(tcb_ptr);
}

static void osBinarySemphrBlockTask(binarySemphrHandle_dtype semphr_handle, u32 ticks){		/* to be called inside any task to suspend it until defined timer ticks pass */
	CRITICAL_SECTION_START();
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
	curr_tcb_ptr->state = BLOCKED;
	curr_tcb_ptr->blocking_semphr_handle = (u32*)semphr_handle;
	curr_tcb_ptr->timer_handler = (u32*)newTimer_ptr;

	newTimer_ptr->function_ptr = osBlockCallBack;
	newTimer_ptr->callback_args = (void*)curr_tcb_ptr;
	newTimer_ptr->u32Ticks = ticks;

	osEnqueueTcbPriority(&(semphr_handle->tcbBlockQueue), curr_tcb_ptr);

	/* register the timer to this callback and this tcb_ptr */
	ostimerRegister(newTimer_ptr);
	/* manage the ready queue to see which task is running next and load it's context to switch immediately */
	osForceContextSwitching();
}
