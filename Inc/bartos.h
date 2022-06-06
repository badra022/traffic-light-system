/*
 * bartos.h
 *
 *  Created on: May 27, 2022
 *      Author: Badra
 */

#ifndef INC_BARTOS_H_
#define INC_BARTOS_H_

typedef enum TaskState{
	RUNNING,
	READY,
	SUSPENDED,
	TERMINATED,
	BLOCKED,
}taskState_dtype;

typedef struct Tcb{
	u32* 			stack_save_ptr;
	struct Tcb* 	next_tcb_ptr;
	struct Tcb* 	prev_tcb_ptr;
	FUNCTION_PTR	task;
	u8				priority;
	taskState_dtype	state;
	u32*			blocking_semphr_handle;
	u8				semphr_get_status;
	u32*			timer_handler;
	u32*			blocking_msg_queue_handle;
	u8				msg_status;
	u8				block_wake_status;
	u8				suspend_wake_status;
}tcb_dtype;

extern tcb_dtype* TcbPtrQueueHead;

tcb_dtype* osGetCurrentTcb(void);
void BARTOS_IntEnterRoutine(void);
void BARTOS_IntExitRoutine(void);
u8 osIsStarted(void);
u8 osResumeTask(tcb_dtype* tcb_ptr);
u8 BARTOS_createTask(FUNCTION_PTR task, u8 priority);
void osForceContextSwitching(void);
void BARTOS_endTask(void);
void BARTOS_start(void);
u8 osEnqueueTcbPriority(tcb_dtype** TcbPtrQueueHead_ptr, tcb_dtype* tcb_ptr);
u8 osDequeueTcbEntry(tcb_dtype** TcbPtrQueueHead_ptr, tcb_dtype* tcb_ptr);
tcb_dtype* osDequeueTcbHead(tcb_dtype** TcbPtrQueueHead_ptr);

#include "bartostimer.h"
#include <semphr.h>
#include <msgqueue.h>


#endif /* INC_BARTOS_H_ */
