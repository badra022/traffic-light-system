/*
 * bartos.h
 *
 *  Created on: May 27, 2022
 *      Author: Badra
 */

#ifndef INC_BARTOS_H_
#define INC_BARTOS_H_

#define MAX_NUMBER_OF_TASKS		10
#define STD_STACK_SIZE			100

typedef enum TaskState{
	RUNNING,
	READY,
	SUSPENDED,
	TERMINATED,
	BLOCKED,
}taskState_dtype;

typedef struct Tcb{
	u32* 			stack_save_ptr;
	struct Tcb* 		next_tcp_ptr;
	struct Tcb* 		prev_tcp_ptr;
	FUNCTION_PTR	task;
	u8				priority;
	taskState_dtype	state;
}tcb_dtype;

extern tcb_dtype* TcbPtrQueueHead;

tcb_dtype* Bartos_getCurrentTcb(void);

void Bartos_createTask(FUNCTION_PTR task, u8 u8Priority);

u8 Bartos_IsStarted(void);

void Bartos_manageTasks(void);

ErrorStatus Bartos_endTask(void);

void Bartos_start(void);

u8	Bartos_isQueueTcbPriorityExist(tcb_dtype** TcbPtrQueueHead_ptr, u8 priority);

u8 Bartos_isQueueTcbPriorityOrHigherExist(tcb_dtype** TcbPtrQueueHead_ptr, u8 priority);

ErrorStatus Bartos_enqueueTcbPriority(tcb_dtype** TcbPtrQueueHead_ptr, tcb_dtype* tcb_ptr);

ErrorStatus Bartos_dequeueTcbEntry(tcb_dtype** TcbPtrQueueHead_ptr, tcb_dtype* tcb_ptr);

u8 Bartos_isQueueEmpty(tcb_dtype** TcbPtrQueueHead_ptr);

tcb_dtype* Bartos_dequeueTcbHead(tcb_dtype** TcbPtrQueueHead_ptr);

ErrorStatus Bartos_resumeTask(tcb_dtype* tcb_ptr);


#endif /* INC_BARTOS_H_ */
