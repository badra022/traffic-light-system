/*
 * smphr.c
 *
 *  Created on: Jun 5, 2022
 *      Author: Badra
 */


#include "stdtypes.h"
#include "macros.h"
#include "port.h"
#include <stdlib.h>

#include "bartos_config.h"
#include "bartos.h"
#include "bartostimer.h"
#include <msgqueue.h>

/* forward Declaration */
static u8 osMessegeQueueBlockTask(msgQueueHandler_dtype queue_handle, u32 ticks);

msgQueueHandler_dtype BARTOS_createQueue(u8* container, u32 capacity){

	msgQueueHandler_dtype msgQueueHandler = (msgQueueHandler_dtype)malloc(sizeof(msgQueue_dtype));
	msgQueueHandler->dataQueue = createQueue(container, capacity);
	msgQueueHandler->receiverBlockingQueue = NULL;
	msgQueueHandler->senderBlockingQueue = NULL;

	return msgQueueHandler;
}

u8 BARTOS_QueueGet(msgQueueHandler_dtype handle, u32 timeout, u8* return_ptr){
	u8 status;
	CRITICAL_SECTION_START();
	tcb_dtype* curr_tcb_ptr = osGetCurrentTcb();
	if(handle == NULL){
		status = ERR_INVALID_PARAMETER;
	}
	else{
		if(isQueueEmpty(handle->dataQueue)){

			if(timeout == (u32)-1){
				/* don't block the task and return status ERR_QUEUE. */
				status = ERR_QUEUE;
			}
			else if(timeout == 0){
				/* add the task to block queue in the msgQueue. */
				curr_tcb_ptr->state = BLOCKED;
				curr_tcb_ptr->msg_status = FALSE;
				osEnqueueTcbPriority(&(handle->receiverBlockingQueue), curr_tcb_ptr);

				osForceContextSwitching();

				status = curr_tcb_ptr->block_wake_status;
			}
			else if(timeout > 0 && timeout <= (15 * SYSTEM_TICKS_PER_SEC)){

				curr_tcb_ptr->msg_status = FALSE;		/* this task will be blocked as a rcvr task */
				status = osMessegeQueueBlockTask(handle, timeout);

			}
		}
		if(status == OK){
			status = dequeue(handle->dataQueue, return_ptr);

			/* check for other tasks blocked for rcv/snd to the queue and wake the task (either sender or receiver)
			 * that have the highest priority, check the senders first as they have more priority to not miss any new data */

			CRITICAL_SECTION_START();

			tcb_dtype* next_tcb_ptr;
			if(isQueueFull(handle->dataQueue)){
				/* switch to the next receiver task */
				next_tcb_ptr = osDequeueTcbHead(&(handle->receiverBlockingQueue));
			}
			else if(isQueueEmpty(handle->dataQueue)){
				/* switch to the next sender task */
				next_tcb_ptr = osDequeueTcbHead(&(handle->senderBlockingQueue));
			}
			else{
				next_tcb_ptr = osDequeueTcbHead(&(handle->senderBlockingQueue));
				if(next_tcb_ptr == NULL){
					next_tcb_ptr = osDequeueTcbHead(&(handle->receiverBlockingQueue));
				}
				else{
					/* we will wake the sender task and make it ready to run */
				}
			}
			if(next_tcb_ptr == NULL){
				/* No other tasks blocked over rcv/snd to the queue */
			}
			else{
				next_tcb_ptr->block_wake_status = OK;
				next_tcb_ptr->blocking_msg_queue_handle = NULL;
				if(next_tcb_ptr->timer_handler != NULL){
					ostimerCancel((bartosTimer_dtype*)(next_tcb_ptr->timer_handler));
				}
				else{
					/* This task wasn't attached to timeout callback for receiving/sending data from/to the queue */
				}

				osResumeTask(next_tcb_ptr);
				if(osGetCurrentTcb()){
					/* don't switch context, we are in an interrupt context */
				}
				else{
					/* we are in thread context, you can switch */
					osForceContextSwitching();
				}
			}
		}
	}
	CRITICAL_SECTION_END();
	return status;
}


u8 BARTOS_QueuePut(msgQueueHandler_dtype handle, u32 timeout, u8 data){
	CRITICAL_SECTION_START();
	tcb_dtype* curr_tcb_ptr;
	u8 status = OK;
	if(handle == NULL){
		status = ERR_INVALID_PARAMETER;
	}
	else{
		if(isQueueFull(handle->dataQueue)){
			if(timeout == (u32)-1){
				/* don't block the task and return MAX_UNSIGNED_LONG. */
				status = ERR_QUEUE;
			}
			else if(timeout == 0){
				curr_tcb_ptr = osGetCurrentTcb();
				curr_tcb_ptr->state = BLOCKED;
				curr_tcb_ptr->msg_status = TRUE;		/* this task will be blocked as a sender task */
				osEnqueueTcbPriority(&(handle->senderBlockingQueue), curr_tcb_ptr);

				osForceContextSwitching();
				status = curr_tcb_ptr->block_wake_status;
			}
			else if(timeout > 0 && timeout <= (15 * SYSTEM_TICKS_PER_SEC)){

				curr_tcb_ptr = osGetCurrentTcb();
				curr_tcb_ptr->msg_status = TRUE;		/* this task will be blocked as a sender task */
				status = osMessegeQueueBlockTask(handle, timeout);
			}
		}
		if(status == OK){
			status = enqueue(handle->dataQueue, data);

			/* check for other tasks blocked for rcv/snd to the queue and wake the task (either sender or receiver)
			 * that have the highest priority, check the senders first as they have more priority to not miss any new data */

			CRITICAL_SECTION_START();

			tcb_dtype* next_tcb_ptr;
			if(isQueueFull(handle->dataQueue)){
				/* switch to the next receiver task */
				next_tcb_ptr = osDequeueTcbHead(&(handle->receiverBlockingQueue));
			}
			else if(isQueueEmpty(handle->dataQueue)){
				/* switch to the next sender task */
				next_tcb_ptr = osDequeueTcbHead(&(handle->senderBlockingQueue));
			}
			else{
				next_tcb_ptr = osDequeueTcbHead(&(handle->senderBlockingQueue));
				if(next_tcb_ptr == NULL){
					next_tcb_ptr = osDequeueTcbHead(&(handle->receiverBlockingQueue));
				}
				else{
					/* we will wake the sender task and make it ready to run */
				}
			}
			if(next_tcb_ptr == NULL){
				/* No other tasks blocked over rcv/snd to the queue */
			}
			else{
				next_tcb_ptr->block_wake_status = OK;
				next_tcb_ptr->blocking_msg_queue_handle = NULL;
				if(next_tcb_ptr->timer_handler != NULL){
					ostimerCancel((bartosTimer_dtype*)(next_tcb_ptr->timer_handler));
				}
				else{
					/* This task wasn't attached to timeout callback for receiving/sending data from/to the queue */
				}

				osResumeTask(next_tcb_ptr);
				if(osGetCurrentTcb() == NULL){
					/* don't switch context, we are in an interrupt context */
				}
				else{
					/* we are in thread context, you can switch */
					osForceContextSwitching();
				}
			}
		}
	}
	CRITICAL_SECTION_END();
	return status;
}

static void osMessegeQueueBlockCallBack(void* tcb_void_ptr){
	/* if the ticks expire, add the tcb to the ready queue */
	/* remove the tcb from the blocking queue of the suspending semaphore */
	tcb_dtype* tcb_ptr = (tcb_dtype*)tcb_void_ptr;
	msgQueueHandler_dtype queue_handle = (msgQueueHandler_dtype)(tcb_ptr->blocking_msg_queue_handle);
	if(tcb_ptr->msg_status == TRUE){
		osDequeueTcbEntry(&(queue_handle->senderBlockingQueue), tcb_ptr);
	}
	else{
		osDequeueTcbEntry(&(queue_handle->receiverBlockingQueue), tcb_ptr);
	}
	tcb_ptr->blocking_msg_queue_handle = NULL;
	tcb_ptr->block_wake_status = ERR_TIMEOUT;
	osResumeTask(tcb_ptr);
}

static u8 osMessegeQueueBlockTask(msgQueueHandler_dtype queue_handle, u32 ticks){		/* to be called inside any task to suspend it until defined timer ticks pass */
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
	curr_tcb_ptr->blocking_msg_queue_handle = (u32*)queue_handle;
	curr_tcb_ptr->timer_handler = (u32*)newTimer_ptr;

	newTimer_ptr->function_ptr = osMessegeQueueBlockCallBack;
	newTimer_ptr->callback_args = (void*)curr_tcb_ptr;
	newTimer_ptr->u32Ticks = ticks;

	if(curr_tcb_ptr->msg_status == TRUE){
		osEnqueueTcbPriority(&(queue_handle->senderBlockingQueue), curr_tcb_ptr);
	}
	else{
		osEnqueueTcbPriority(&(queue_handle->receiverBlockingQueue), curr_tcb_ptr);
	}


	/* register the timer to this callback and this tcb_ptr */
	ostimerRegister(newTimer_ptr);
	/* manage the ready queue to see which task is running next and load it's context to switch immediately */
	osForceContextSwitching();

	return curr_tcb_ptr->block_wake_status;
}
