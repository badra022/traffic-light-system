/*
 * bartos_kernel.c
 *
 *  Created on: May 27, 2022
 *      Author: Badra
 */

#include "stdtypes.h"
#include "macros.h"
#include "port.h"

#include "bartos_config.h"
#include "bartos.h"
#include "bartostimer.h"

tcb_dtype tcbs[MAX_NUMBER_OF_TASKS] = {0};		/* store the physical TCBs for each task in the RAM */
tcb_dtype* TcbPtrQueueHead = NULL;
u32 tcb_stack[MAX_NUMBER_OF_TASKS][STD_STACK_SIZE];		/* defines stack of 100 4bytes word for each task in the RAM */
static tcb_dtype* curr_tcb_ptr = NULL;
static u32* volatile curr_return_address = NULL;
static u8 isStarted = 0;

/* Forward Declaration */
__attribute__((naked)) void LaunchScheduler(void);
static void Bartos_idleTask(void);
static u8 Bartos_getNextTcbIndex(void);
static void initTaskStack(u8 index);
static void Bartos_manageTasks(void);
static u8 Bartos_isQueueTcbPriorityOrHigherExist(tcb_dtype** TcbPtrQueueHead_ptr, u8 priority);
static u8 Bartos_isQueueEmpty(tcb_dtype** TcbPtrQueueHead_ptr);

tcb_dtype* Bartos_getCurrentTcb(void){
	return curr_tcb_ptr;
}

u8 Bartos_IsStarted(void){
	return isStarted;
}

u8 Bartos_resumeTask(tcb_dtype* tcb_ptr){
	u8 status;
	if(tcb_ptr == NULL){
		status = ERR_INVALID_PARAMETER;
	}
	else{

		if(Bartos_enqueueTcbPriority(&TcbPtrQueueHead, tcb_ptr) == OK){
			tcb_ptr->state = READY;
			status = OK;
		}
		else{
			status = ERR_ENQUEUE;
		}
	}
	return status;
}



u8 Bartos_createTask(FUNCTION_PTR task, u8 priority){
	u8 status = ERR_FAILED_TO_PERFORM;
	if(task == NULL){
		status = ERR_INVALID_PARAMETER;
	}
	else if(priority <= 0){
		status = ERR_INVALID_PARAMETER;
	}
	else{
		u8 task_idx = Bartos_getNextTcbIndex();
		if(task_idx == MAX_UNSIGNED_CHARACTER){
			status = ERR_INVALID_ARR_INDEX;
		}
		else{
			tcbs[task_idx].priority = priority;
			tcbs[task_idx].state = READY;
			tcbs[task_idx].task = task;
			initTaskStack(task_idx);
			/* add the task to ready to run tasks queue */
			status = Bartos_enqueueTcbPriority(&TcbPtrQueueHead, &tcbs[task_idx]);
		}
	}
	return status;
}





void Bartos_forceContextSwitch(void){	/* this function won't be called from interrupt context */
	CRITICAL_SECTION_END();
	SVC_INTERRUPT_TRIGGER();
}

void Bartos_endTask(void){
	curr_tcb_ptr->state = TERMINATED;
	Bartos_dequeueTcbEntry(&TcbPtrQueueHead, curr_tcb_ptr);
	Bartos_forceContextSwitch();
}



void Bartos_start(void){
	isStarted = TRUE;
	Bartos_createTask(Bartos_idleTask, MAX_TASK_PRIORITY);
	BartosTimer_Init();
	curr_tcb_ptr = Bartos_dequeueTcbHead(&TcbPtrQueueHead);
	LaunchScheduler();
}



u8 Bartos_enqueueTcbPriority(tcb_dtype** TcbPtrQueueHead_ptr, tcb_dtype* tcb_ptr){
	u8 status = ERR_FAILED_TO_PERFORM;
	if(TcbPtrQueueHead_ptr == NULL || tcb_ptr == NULL){	/* invalid parameter */
		status = ERR_INVALID_PARAMETER;
	}
	else{
		if(*TcbPtrQueueHead_ptr == NULL){ /* empty queue */
			*TcbPtrQueueHead_ptr = tcb_ptr;
			(*TcbPtrQueueHead_ptr)->next_tcp_ptr = NULL;
			(*TcbPtrQueueHead_ptr)->prev_tcp_ptr = NULL;
			status = OK;
		}
		else if((*TcbPtrQueueHead_ptr)->next_tcp_ptr == NULL && (*TcbPtrQueueHead_ptr)->prev_tcp_ptr == NULL){
			/* there is only the head node */
			if(tcb_ptr->priority < (*TcbPtrQueueHead_ptr)->priority){	/* this tcb have higher priority than the head */
				(*TcbPtrQueueHead_ptr)->prev_tcp_ptr = tcb_ptr;
				(*TcbPtrQueueHead_ptr)->next_tcp_ptr = NULL;
				tcb_ptr->next_tcp_ptr = *TcbPtrQueueHead_ptr;
				tcb_ptr->prev_tcp_ptr = NULL;
				*TcbPtrQueueHead_ptr = tcb_ptr;
				status = OK;
			}
			else{		/* this tcb have lower priority than the current head */
				(*TcbPtrQueueHead_ptr)->prev_tcp_ptr = NULL;
				(*TcbPtrQueueHead_ptr)->next_tcp_ptr = tcb_ptr;
				tcb_ptr->next_tcp_ptr = NULL;
				tcb_ptr->prev_tcp_ptr = *TcbPtrQueueHead_ptr;
				status = OK;
			}
		}
		else{
			if(tcb_ptr->priority < (*TcbPtrQueueHead_ptr)->priority){	/* this tcb have higher priority than the head */
				(*TcbPtrQueueHead_ptr)->prev_tcp_ptr = tcb_ptr;
				tcb_ptr->next_tcp_ptr = *TcbPtrQueueHead_ptr;
				tcb_ptr->prev_tcp_ptr = NULL;
				*TcbPtrQueueHead_ptr = tcb_ptr;
				status = OK;
			}
			else{
				tcb_dtype* curr_tcb = (*TcbPtrQueueHead_ptr)->next_tcp_ptr;
				while(curr_tcb){
					if(tcb_ptr->priority < curr_tcb->priority){
						tcb_ptr->prev_tcp_ptr = curr_tcb->prev_tcp_ptr;
						curr_tcb->prev_tcp_ptr->next_tcp_ptr = tcb_ptr;
						curr_tcb->prev_tcp_ptr = tcb_ptr;
						tcb_ptr->next_tcp_ptr = curr_tcb;
						curr_tcb = NULL;		/* End while loop */
						status = OK;
					}
					else{
						if(curr_tcb->next_tcp_ptr == NULL){
							curr_tcb->next_tcp_ptr = tcb_ptr;
							tcb_ptr->prev_tcp_ptr = curr_tcb;
							tcb_ptr->next_tcp_ptr = NULL;
							curr_tcb = NULL;	/* End while loop */
							status = OK;
						}
						else{
							curr_tcb = curr_tcb->next_tcp_ptr;
						}
					}
				}
			}
		}
	}
	return status;
}

u8 Bartos_dequeueTcbEntry(tcb_dtype** TcbPtrQueueHead_ptr, tcb_dtype* tcb_ptr){
	u8 status = ERR_FAILED_TO_PERFORM;
	if(TcbPtrQueueHead_ptr == NULL || tcb_ptr == NULL){	/* invalid parameter */
		status = ERR_INVALID_PARAMETER;
	}
	else if(*TcbPtrQueueHead_ptr == NULL){ /* empty queue */
		status = ERR_EMPTY_QUEUE;
	}
	else{
		tcb_dtype* curr_tcb_ptr = (*TcbPtrQueueHead_ptr);
		while(curr_tcb_ptr){
			if(curr_tcb_ptr == tcb_ptr){
				if(curr_tcb_ptr->next_tcp_ptr != NULL){
					curr_tcb_ptr->next_tcp_ptr->prev_tcp_ptr = curr_tcb_ptr->prev_tcp_ptr;
				}
				else{
					/* this is the last tcb in the queue */
				}
				if(curr_tcb_ptr->prev_tcp_ptr != NULL){
					curr_tcb_ptr->prev_tcp_ptr->next_tcp_ptr = curr_tcb_ptr->next_tcp_ptr;
				}
				else{
					/* this is the first (head) tcb in the queue */
				}
				if(curr_tcb_ptr == *TcbPtrQueueHead_ptr){
					*TcbPtrQueueHead_ptr = curr_tcb_ptr->next_tcp_ptr;
				}
				curr_tcb_ptr = NULL;		/* End while loop */
				status = OK;
			}
			else{
				curr_tcb_ptr = curr_tcb_ptr->next_tcp_ptr;
			}
		}
	}
	return status;
}



tcb_dtype* Bartos_dequeueTcbHead(tcb_dtype** TcbPtrQueueHead_ptr){
	tcb_dtype* dequeued_tcb_ptr = NULL;
	if(TcbPtrQueueHead_ptr == NULL){
		/* dequeued_tcb_ptr is NULL */
	}
	else if((*TcbPtrQueueHead_ptr) == NULL){
		/* dequeued_tcb_ptr is NULL */
	}
	else{
		dequeued_tcb_ptr = (*TcbPtrQueueHead_ptr);
		(*TcbPtrQueueHead_ptr) = (*TcbPtrQueueHead_ptr)->next_tcp_ptr;
		if(*TcbPtrQueueHead_ptr != NULL){
			(*TcbPtrQueueHead_ptr)->prev_tcp_ptr = NULL;
		}
		else{
			/* queue is empty now */
		}
		dequeued_tcb_ptr->next_tcp_ptr = dequeued_tcb_ptr->prev_tcp_ptr = NULL;
	}
	return dequeued_tcb_ptr;
}

/* to understand the role of this function you need to know that
 * context switching, which is pushing (R0-R12, LR,PC,PSR) context is done for interrupts only,
 * calling a regular function is not considered a separate context ,
 * it just builds up it's variables on top of the caller function and edit its caller's PSR, PC and LR.
 * so in order to start our threads with some task, we need to initiate it's whole context ourselves,
 * so that the task start running on it's own context,
 * that's the role of this LaunchScheduler function.
 */
__attribute__((naked)) void LaunchScheduler(void)
{
    /* R0 contains the address of curr_tcb_ptr */
    __asm("LDR     R0, =curr_tcb_ptr");

    /* R2 contains the address in curr_tcb_ptr(value of curr_tcb_ptr) */
    __asm("LDR     R2, [R0]");

    /* Load the SP register with the stacked SP value */
    __asm("LDR     R4, [R2]");
    __asm("MOV     SP, R4");

    /* Pop registers R8-R11 (user saved context) */
    __asm("POP     {R4-R7}");
    __asm("MOV     R8, R4");
    __asm("MOV     R9, R5");
    __asm("MOV     R10, R6");
    __asm("MOV     R11, R7");
    /* Pop registers R4-R7 (user saved context) */
    __asm("POP     {R4-R7}");
    /*  Start popping the stacked exception frame. (R0-R3, R12, LR, PC, PSR) */
    __asm("POP     {R0-R3}");
    __asm("POP     {R4}");
    __asm("MOV     R12, R4");

    /* Skip the saved LR */
    __asm("ADD     SP,SP,#4");

    /* POP the saved PC into LR via R4, We do this to jump into the
     first task when we execute the branch instruction to exit this routine. */
    __asm("POP     {R4}");
    __asm("MOV     LR, R4");

    __asm("ADD     SP,SP,#4");		/* to align the SP at the beginning of the Stack region of the Task */

    CRITICAL_SECTION_END();
    __asm("BX      LR");		/* go to our first Task */
}

__attribute__((naked))
void SysTick_Handler(void){
	/* STEP 1 - SAVE THE CURRENT TASK CONTEXT

	At this point the processor has already pushed PSR, PC, LR, R12, R3, R2, R1 and R0
	onto the stack. We need to push the rest(i.e R4, R5, R6, R7, R8, R9, R10 & R11) to save the
	context of the current task. */

	CRITICAL_SECTION_START();
    __asm("PUSH    {R4-R7}");				/* Push registers R4 to R7 */

    /* Push registers R8-R11 through storing them in R4-R7 then pushing R4-R7 into stack */
    __asm("MOV     R4, R8");
    __asm("MOV     R5, R9");
    __asm("MOV     R6, R10");
    __asm("MOV     R7, R11");
    __asm("PUSH    {R4-R7}");

    __asm("LDR     R0, =curr_tcb_ptr");	/* load R0 with the address of curr_tcb_ptr (R0 = &curr_tcb_ptr) */

    __asm("LDR     R1, [R0]");			/* Load R1 with the content of curr_tcb_ptr (R1 = *(R0))
								(i.e post this, R1 will contain the address of current TCB) */

	/* updating current task's stack pointer in it's tcb */
    __asm("MOV     R4, SP");				/* Move the SP value to R4 */
    __asm("STR     R4, [R1]");			/* Store the value of the stack pointer(copied in R4) to the stack_ptr element in current task's TCB
								  This marks an end to saving the context of the current task. */
    __asm("LDR	   R0, =curr_return_address");
    __asm("STR     LR, [R0]");
	/* ------------------------------------------------------------------------------------------------------ */

    BartosTimer_TimerTick();
	Bartos_manageTasks();		/* UPDATE curr_tcb_ptr to point to next task's tcb decided by the scheduler */

	/* ------------------------------------------------------------------------------------------------------
      STEP 2: LOAD THE NEW TASK CONTEXT FROM ITS STACK TO THE CPU REGISTERS. */

	__asm("LDR     R0, =curr_tcb_ptr");	/* load R0 with the address of curr_tcb_ptr (R0 = &curr_tcb_ptr) */

	__asm("LDR     R1, [R0]");			/* Load R1 with the content of curr_tcb_ptr (R1 = *(R0))
								 (i.e post this, R1 will contain the address of current TCB) */

    /* Load the newer tasks TCB to the SP using R4. */
	__asm("LDR     R4, [R1]");
	__asm("MOV     SP, R4");

    /* Pop registers R8-R11 */
	__asm("POP     {R4-R7}");
	__asm("MOV     R8, R4");
	__asm("MOV     R9, R5");
	__asm("MOV     R10, R6");
	__asm("MOV     R11, R7");

	/* Pop registers R4-R7 */
	__asm("POP     {R4-R7}");

    /* Return from interrupt */
    __asm("LDR	   R0, =curr_return_address");
    __asm("LDR     LR, [R0]");

    CRITICAL_SECTION_END();

	__asm("BX      LR");
    /* what happens after returning from this exception (interrupt) is that the processor pop R0-R3, R12, LR, PC and PSR
     from the current pointed stack (we edited it to point to next task's stack) into their original registers
     PC is also popped among the context so the processor goes to the location of next task pointed by it's PC it just popped
     we already popped R4-R11. with the automatically popped context (R0-R3, R12, LR, PSR),
     the context is now complete upon task execution */
}


__attribute__((naked))
void SVC_Handler(void){
	/* STEP 1 - SAVE THE CURRENT TASK CONTEXT

	At this point the processor has already pushed PSR, PC, LR, R12, R3, R2, R1 and R0
	onto the stack. We need to push the rest(i.e R4, R5, R6, R7, R8, R9, R10 & R11) to save the
	context of the current task. */

    CRITICAL_SECTION_START();
    __asm("PUSH    {R4-R7}");				/* Push registers R4 to R7 */

    /* Push registers R8-R11 through storing them in R4-R7 then pushing R4-R7 into stack */
    __asm("MOV     R4, R8");
    __asm("MOV     R5, R9");
    __asm("MOV     R6, R10");
    __asm("MOV     R7, R11");
    __asm("PUSH    {R4-R7}");

    __asm("LDR     R0, =curr_tcb_ptr");	/* load R0 with the address of curr_tcb_ptr (R0 = &curr_tcb_ptr) */

    __asm("LDR     R1, [R0]");			/* Load R1 with the content of curr_tcb_ptr (R1 = *(R0))
								(i.e post this, R1 will contain the address of current TCB) */

	/* updating current task's stack pointer in it's tcb */
    __asm("MOV     R4, SP");				/* Move the SP value to R4 */
    __asm("STR     R4, [R1]");			/* Store the value of the stack pointer(copied in R4) to the stack_ptr element in current task's TCB
								  This marks an end to saving the context of the current task. */
    __asm("LDR	   R0, =curr_return_address");
    __asm("STR     LR, [R0]");
	/* ------------------------------------------------------------------------------------------------------ */

	Bartos_manageTasks();		/* UPDATE curr_tcb_ptr to point to next task's tcb decided by the scheduler */

	/* ------------------------------------------------------------------------------------------------------
      STEP 2: LOAD THE NEW TASK CONTEXT FROM ITS STACK TO THE CPU REGISTERS. */

	__asm("LDR     R0, =curr_tcb_ptr");	/* load R0 with the address of curr_tcb_ptr (R0 = &curr_tcb_ptr) */

	__asm("LDR     R1, [R0]");			/* Load R1 with the content of curr_tcb_ptr (R1 = *(R0))
								 (i.e post this, R1 will contain the address of current TCB) */

    /* Load the newer tasks TCB to the SP using R4. */
	__asm("LDR     R4, [R1]");
	__asm("MOV     SP, R4");

    /* Pop registers R8-R11 */
	__asm("POP     {R4-R7}");
	__asm("MOV     R8, R4");
	__asm("MOV     R9, R5");
	__asm("MOV     R10, R6");
	__asm("MOV     R11, R7");

	/* Pop registers R4-R7 */
	__asm("POP     {R4-R7}");

    /* Return from interrupt */
    __asm("LDR	   R0, =curr_return_address");
    __asm("LDR     LR, [R0]");

    CRITICAL_SECTION_END();

    __asm("BX      LR");
    /* what happens after returning from this exception (interrupt) is that the processor pop R0-R3, R12, LR, PC and PSR
     from the current pointed stack (we edited it to point to next task's stack) into their original registers
     PC is also popped among the context so the processor goes to the location of next task pointed by it's PC it just popped
     we already popped R4-R11. with the automatically popped context (R0-R3, R12, LR, PSR),
     the context is now complete upon task execution */
}


static void Bartos_idleTask(void){
	while(1){}
}

static u8 Bartos_getNextTcbIndex(void){
	for(u8 idx = 0; idx < MAX_NUMBER_OF_TASKS; idx++){
		if(tcbs[idx].state == TERMINATED || tcbs[idx].stack_save_ptr == NULL){
			return idx;
		}
	}
	return MAX_UNSIGNED_CHARACTER;		/* didn't find any space for new Tcb allocation */
}

static void initTaskStack(u8 index){
	tcbs[index].stack_save_ptr = &tcb_stack[index][STD_STACK_SIZE - 16];
	tcb_stack[index][STD_STACK_SIZE - 1] = 0x01000000;
	tcb_stack[index][STD_STACK_SIZE - 2] = (u32)tcbs[index].task;
	for(u16 idx = 0; idx < (STD_STACK_SIZE - 2); idx++){
		tcb_stack[index][idx] = 0;
	}
}

/* this function is called at the systick tick interrupt to amnage the tasks and update curr_tcb_ptr to point to
 * next task's tcb block before context switching */
static void Bartos_manageTasks(void){
	if(curr_tcb_ptr->state != READY){
		/* switch to this task that have higher or same priority */
		/* idle is guaranteed to be always in the ready queue and always is ready */
		curr_tcb_ptr = Bartos_dequeueTcbHead(&TcbPtrQueueHead);
	}
	else{
		if(!Bartos_isQueueEmpty(&TcbPtrQueueHead)){
			/* see if there is another same priority task to share the timeslicing with */
			if(Bartos_isQueueTcbPriorityOrHigherExist(&TcbPtrQueueHead, curr_tcb_ptr->priority)){
				/* switch to this task that have higher or same priority */
				tcb_dtype* next_tcb_ptr = Bartos_dequeueTcbHead(&TcbPtrQueueHead);
				Bartos_enqueueTcbPriority(&TcbPtrQueueHead, curr_tcb_ptr);
				curr_tcb_ptr = next_tcb_ptr;
			}
			else{
				/* if not found, do nothing */
			}
		}
	}
}

static u8 Bartos_isQueueEmpty(tcb_dtype** TcbPtrQueueHead_ptr){
	return ((*TcbPtrQueueHead_ptr) == NULL);
}

static u8 Bartos_isQueueTcbPriorityOrHigherExist(tcb_dtype** TcbPtrQueueHead_ptr, u8 priority){
	u8 status = FALSE;
	if(TcbPtrQueueHead_ptr == NULL){	/* invalid parameter */
			status = ERR_INVALID_PARAMETER;
	}
	else{
		status = ((*TcbPtrQueueHead_ptr)->priority <= priority);
		if(status){
			status = TRUE;
		}
		else{
			/* leave it */
		}
	}
	return status;
}
