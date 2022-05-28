/*
 * bartos.h
 *
 *  Created on: May 27, 2022
 *      Author: Badra
 */

#ifndef INC_BARTOS_H_
#define INC_BARTOS_H_

#define MAX_NUMBER_OF_TASKS		3
#define STD_STACK_SIZE			1000

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

void Bartos_createTask(FUNCTION_PTR task, u8 u8Priority);
void Bartos_start(void);

#endif /* INC_BARTOS_H_ */
