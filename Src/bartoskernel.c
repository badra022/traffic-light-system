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
static u8 osIntCnt = 0;

/* Forward Declaration */
__attribute__((naked)) void LaunchScheduler(void);
static void osIdleTask(void);
static u8 osGetNextTcbIndex(void);
static void osInitTaskStack(u8 index);
static void osManageTasks(void);
static u8 osIsQueueTcbPriorityOrHigherExist(tcb_dtype** TcbPtrQueueHead_ptr, u8 priority);
static u8 osIsQueueEmpty(tcb_dtype** TcbPtrQueueHead_ptr);

/*
 * \b getCurrentTcb
 *
 * @return Pointer to the current tcb on execution
 */
tcb_dtype* osGetCurrentTcb(void){
	if(osIntCnt == 0){
		return curr_tcb_ptr;
	}
	else{
		return NULL;
	}
}

/*
 * \b BARTOS_IntEnterRoutine
 *
 * This API must be called at the beginning of any interrupt that wish to use BARTOS primitive and may change any task's state
 *
 */
void BARTOS_IntEnterRoutine(void){
	CRITICAL_SECTION_START();
	osIntCnt++;
}

/*
 * \b BARTOS_IntExitRoutine
 *
 * This API must be called at the end of any interrupt that wish to use BARTOS primitive and may change any task's state
 *
 */
void BARTOS_IntExitRoutine(void){
	CRITICAL_SECTION_END();
	osIntCnt--;
}

/*
 * \b osIsStarted
 *
 * @retval TRUE started
 * @retval FALSE not started yet
 */
u8 osIsStarted(void){
	return isStarted;
}


/*
 * \b osResumeTask
 *
 * Wakes up any suspended or blocked task passed to it.
 *
 * This function is called using delay, semaphore and mutex callback functions
 * to wake up the task after some event or timeout that was blocked or suspended by them.
 *
 * @param[in] Pointer to task's tcb block
 *
 * @retval ERR_INVALID_PARAMETER invalid parameter values.
 * @retval ERR_ENQUEUE couldn't put the task in the ready queue.
 * @retval OK success
 */
u8 osResumeTask(tcb_dtype* tcb_ptr){
	u8 status;
	if(tcb_ptr == NULL){
		status = ERR_INVALID_PARAMETER;
	}
	else{

		if(osEnqueueTcbPriority(&TcbPtrQueueHead, tcb_ptr) == OK){
			tcb_ptr->state = READY;
			status = OK;
		}
		else{
			status = ERR_ENQUEUE;
		}
	}
	return status;
}


/*
 * \b BARTOS_createTask
 *
 * Creates the TCB block for the passed task handler(ptr to function) with associated priority.
 *
 * This function initiates an TCB block in the array of TCBs to be used for the task handler passed to it
 * and init the neccessary stack space for that TCB and append the TCB to the ready queue according to it's priority.
 *
 * Context switching isn't performed right after creating the task, you must wait for the systick_handler to do it.
 *
 * @param[in] task Function handler of the task (pointer to function taking void and returning void)
 * @param[in] priority The priority of executing this task over other tasks (1 is the highest, 0 is prevented)
 *
 * @retval ERR_INVALID_PARAMETER Invalid parameters
 * @retval ERR_INVALID_ARR_INDEX failed to allocate space for the TCB in TCBs array (max number of TCBs had been reached)
 * @retval OK success
 * @retval ERR_FAILED_TO_PERFORM another runtime error happened
 */
u8 BARTOS_createTask(FUNCTION_PTR task, u8 priority){
	u8 status = ERR_FAILED_TO_PERFORM;
	if(task == NULL){
		status = ERR_INVALID_PARAMETER;
	}
	else if(priority <= 0){
		status = ERR_INVALID_PARAMETER;
	}
	else{
		u8 task_idx = osGetNextTcbIndex();
		if(task_idx == MAX_UNSIGNED_CHARACTER){
			status = ERR_INVALID_ARR_INDEX;
		}
		else{
			tcbs[task_idx].priority = priority;
			tcbs[task_idx].state = READY;
			tcbs[task_idx].task = task;
			tcbs[task_idx].blocking_semphr_handle = NULL;
			tcbs[task_idx].semphr_get_status = FALSE;
			tcbs[task_idx].timer_handler = NULL;
			tcbs[task_idx].blocking_msg_queue_handle = NULL;
			osInitTaskStack(task_idx);
			/* add the task to ready to run tasks queue */
			status = osEnqueueTcbPriority(&TcbPtrQueueHead, &tcbs[task_idx]);
		}
	}
	return status;
}




/*
 * \b forceContextSwitching
 *
 * Performs Context switching upon invocation
 *
 * this function re-enables the interrupts and perform SW interrupt using SVC instruction
 * to trigger supervisor handler (SVC_Handler) that is responsible for saving current context,
 * calling \b manageTasks to switch the current TCB to the Highest priority next ready up to run TCB and load it's context.
 *
 */
void osForceContextSwitching(void){	/* this function won't be called from interrupt context */
	CRITICAL_SECTION_END();
	SVC_INTERRUPT_TRIGGER();
}

/*
 * \b BARTOS_endTask
 *
 * Performs Exit routine for the Task calling it.
 *
 * This function make task's TCB at \b TERMINATED state and call \b forceContextSwitching
 * to switch to the next highest priority task.
 *
 */
void BARTOS_endTask(void){
	curr_tcb_ptr->state = TERMINATED;
	osForceContextSwitching();
}


/*
 * \b BARTOS_start
 *
 * Starts the RTOS.
 *
 * This function sets the \b isStarted flag to TRUE and create an TCB for the internal IDLE task in the rtos,
 * then call \b BartosTimer_Init to initiate the timer ticks to enable (Systick_Handler) interrupt each tick,
 * then dequeue the top priority TCB from the ready queue to start execution then call \b LaunchScheduler
 * to jump into the first Task.
 *
 */
void BARTOS_start(void){
	isStarted = TRUE;
	BARTOS_createTask(osIdleTask, MAX_TASK_PRIORITY);
	ostimerInit();
	curr_tcb_ptr = osDequeueTcbHead(&TcbPtrQueueHead);
	LaunchScheduler();
}


/*
 * \b osEnqueueTcbPriority
 *
 * Inserts the passed TCB into the ready Queue of TCBs based on it's priority, the head is the highest priority
 * and tasks are sorted in descending priorities.
 *
 * This function inserts th TCB to the queue based on it's priority, the queue is doubly linked-list implemented.
 * case 1: if the queue is empty (head is NULL), put the TCB as the head
 * case 2: if the queue has only the head, see if the TCB inserted have higher priority or not, if it's higher it
 * should be the new head, if it's not it should be the next node after the head.
 * case 3: if there're more than the head node, loop over the nodes and see which TCB is lower priority than th passed TCB then
 * put the passed TCB after it.
 *
 * if some TCB have the same priority as the passed TCB, it should be inserted after it to preserve FIFO order between
 * similar priority tasks, first come first out.
 *
 * @param[in] TcbPtrQueueHead_ptr Pointer to the Head of the ready Queue
 * @param[in] tcb_ptr Pointer to the tcb that will be inserted
 *
 * @retval ERR_INVALID_PARAMETER invalid parameters
 * @retval OK success
 * @retval ERR_FAILED_TO_PERFORM another runtime error
 *
 */
u8 osEnqueueTcbPriority(tcb_dtype** TcbPtrQueueHead_ptr, tcb_dtype* tcb_ptr){
	u8 status = ERR_FAILED_TO_PERFORM;
	if(TcbPtrQueueHead_ptr == NULL || tcb_ptr == NULL){	/* invalid parameter */
		status = ERR_INVALID_PARAMETER;
	}
	else{
		if(*TcbPtrQueueHead_ptr == NULL){ /* empty queue */
			*TcbPtrQueueHead_ptr = tcb_ptr;
			(*TcbPtrQueueHead_ptr)->next_tcb_ptr = NULL;
			(*TcbPtrQueueHead_ptr)->prev_tcb_ptr = NULL;
			status = OK;
		}
		else if((*TcbPtrQueueHead_ptr)->next_tcb_ptr == NULL && (*TcbPtrQueueHead_ptr)->prev_tcb_ptr == NULL){
			/* there is only the head node */
			if(tcb_ptr->priority < (*TcbPtrQueueHead_ptr)->priority){	/* this tcb have higher priority than the head */
				(*TcbPtrQueueHead_ptr)->prev_tcb_ptr = tcb_ptr;
				(*TcbPtrQueueHead_ptr)->next_tcb_ptr = NULL;
				tcb_ptr->next_tcb_ptr = *TcbPtrQueueHead_ptr;
				tcb_ptr->prev_tcb_ptr = NULL;
				*TcbPtrQueueHead_ptr = tcb_ptr;
				status = OK;
			}
			else{		/* this tcb have lower priority than the current head */
				(*TcbPtrQueueHead_ptr)->prev_tcb_ptr = NULL;
				(*TcbPtrQueueHead_ptr)->next_tcb_ptr = tcb_ptr;
				tcb_ptr->next_tcb_ptr = NULL;
				tcb_ptr->prev_tcb_ptr = *TcbPtrQueueHead_ptr;
				status = OK;
			}
		}
		else{
			if(tcb_ptr->priority < (*TcbPtrQueueHead_ptr)->priority){	/* this tcb have higher priority than the head */
				(*TcbPtrQueueHead_ptr)->prev_tcb_ptr = tcb_ptr;
				tcb_ptr->next_tcb_ptr = *TcbPtrQueueHead_ptr;
				tcb_ptr->prev_tcb_ptr = NULL;
				*TcbPtrQueueHead_ptr = tcb_ptr;
				status = OK;
			}
			else{
				tcb_dtype* curr_tcb = (*TcbPtrQueueHead_ptr)->next_tcb_ptr;
				while(curr_tcb){
					if(tcb_ptr->priority < curr_tcb->priority){
						tcb_ptr->prev_tcb_ptr = curr_tcb->prev_tcb_ptr;
						curr_tcb->prev_tcb_ptr->next_tcb_ptr = tcb_ptr;
						curr_tcb->prev_tcb_ptr = tcb_ptr;
						tcb_ptr->next_tcb_ptr = curr_tcb;
						curr_tcb = NULL;		/* End while loop */
						status = OK;
					}
					else{
						if(curr_tcb->next_tcb_ptr == NULL){
							curr_tcb->next_tcb_ptr = tcb_ptr;
							tcb_ptr->prev_tcb_ptr = curr_tcb;
							tcb_ptr->next_tcb_ptr = NULL;
							curr_tcb = NULL;	/* End while loop */
							status = OK;
						}
						else{
							curr_tcb = curr_tcb->next_tcb_ptr;
						}
					}
				}
			}
		}
	}
	return status;
}

u8 osDequeueTcbEntry(tcb_dtype** TcbPtrQueueHead_ptr, tcb_dtype* tcb_ptr){
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
				if(curr_tcb_ptr->next_tcb_ptr != NULL){
					curr_tcb_ptr->next_tcb_ptr->prev_tcb_ptr = curr_tcb_ptr->prev_tcb_ptr;
				}
				else{
					/* this is the last tcb in the queue */
				}
				if(curr_tcb_ptr->prev_tcb_ptr != NULL){
					curr_tcb_ptr->prev_tcb_ptr->next_tcb_ptr = curr_tcb_ptr->next_tcb_ptr;
				}
				else{
					/* this is the first (head) tcb in the queue */
				}
				if(curr_tcb_ptr == *TcbPtrQueueHead_ptr){
					*TcbPtrQueueHead_ptr = curr_tcb_ptr->next_tcb_ptr;
				}
				curr_tcb_ptr = NULL;		/* End while loop */
				status = OK;
			}
			else{
				curr_tcb_ptr = curr_tcb_ptr->next_tcb_ptr;
			}
		}
	}
	return status;
}



tcb_dtype* osDequeueTcbHead(tcb_dtype** TcbPtrQueueHead_ptr){
	tcb_dtype* dequeued_tcb_ptr = NULL;
	if(TcbPtrQueueHead_ptr == NULL){
		/* dequeued_tcb_ptr is NULL */
	}
	else if((*TcbPtrQueueHead_ptr) == NULL){
		/* dequeued_tcb_ptr is NULL */
	}
	else{
		dequeued_tcb_ptr = (*TcbPtrQueueHead_ptr);
		(*TcbPtrQueueHead_ptr) = (*TcbPtrQueueHead_ptr)->next_tcb_ptr;
		if(*TcbPtrQueueHead_ptr != NULL){
			(*TcbPtrQueueHead_ptr)->prev_tcb_ptr = NULL;
		}
		else{
			/* queue is empty now */
		}
		dequeued_tcb_ptr->next_tcb_ptr = dequeued_tcb_ptr->prev_tcb_ptr = NULL;
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

    ostimerTick();
	osManageTasks();		/* UPDATE curr_tcb_ptr to point to next task's tcb decided by the scheduler */

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

	osManageTasks();		/* UPDATE curr_tcb_ptr to point to next task's tcb decided by the scheduler */

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


static void osIdleTask(void){
	while(1){}
}

static u8 osGetNextTcbIndex(void){
	for(u8 idx = 0; idx < MAX_NUMBER_OF_TASKS; idx++){
		if(tcbs[idx].state == TERMINATED || tcbs[idx].stack_save_ptr == NULL){
			return idx;
		}
	}
	return MAX_UNSIGNED_CHARACTER;		/* didn't find any space for new Tcb allocation */
}

static void osInitTaskStack(u8 index){
	tcbs[index].stack_save_ptr = &tcb_stack[index][STD_STACK_SIZE - 16];
	tcb_stack[index][STD_STACK_SIZE - 1] = 0x01000000;
	tcb_stack[index][STD_STACK_SIZE - 2] = (u32)tcbs[index].task;
	for(u16 idx = 0; idx < (STD_STACK_SIZE - 2); idx++){
		tcb_stack[index][idx] = 0;
	}
}

/* this function is called at the systick tick interrupt to amnage the tasks and update curr_tcb_ptr to point to
 * next task's tcb block before context switching */
static void osManageTasks(void){
	if(curr_tcb_ptr->state != READY){
		/* switch to this task that have higher or same priority */
		/* idle is guaranteed to be always in the ready queue and always is ready */
		curr_tcb_ptr = osDequeueTcbHead(&TcbPtrQueueHead);
	}
	else{
		if(!osIsQueueEmpty(&TcbPtrQueueHead)){
			/* see if there is another same priority task to share the timeslicing with */
			if(osIsQueueTcbPriorityOrHigherExist(&TcbPtrQueueHead, curr_tcb_ptr->priority)){
				/* switch to this task that have higher or same priority */
				tcb_dtype* next_tcb_ptr = osDequeueTcbHead(&TcbPtrQueueHead);
				osEnqueueTcbPriority(&TcbPtrQueueHead, curr_tcb_ptr);
				curr_tcb_ptr = next_tcb_ptr;
			}
			else{
				/* if not found, do nothing */
			}
		}
	}
}

static u8 osIsQueueEmpty(tcb_dtype** TcbPtrQueueHead_ptr){
	return ((*TcbPtrQueueHead_ptr) == NULL);
}

static u8 osIsQueueTcbPriorityOrHigherExist(tcb_dtype** TcbPtrQueueHead_ptr, u8 priority){
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
