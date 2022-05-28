/*
 * bartos_kernel.c
 *
 *  Created on: May 27, 2022
 *      Author: Badra
 */

#include <stdtypes.h>
#include "MATH_macros.h"

#include "systick.h"
#include "bartos.h"

#include "gpio.h"

tcb_dtype tcbs[MAX_NUMBER_OF_TASKS];

tcb_dtype* curr_tcb_ptr = NULL;

u32 tcb_stack[MAX_NUMBER_OF_TASKS][STD_STACK_SIZE];		/* defines stack of 100 4bytes word for each task in the RAM */

u32* curr_return_address = NULL;

__attribute__((naked)) void LaunchScheduler(void);

void Bartos_createTask(FUNCTION_PTR task, u8 u8Priority){
	static u8 task_idx = 0;
	if(task_idx >= MAX_NUMBER_OF_TASKS){
		return;
	}
	else
	{
		/* process to the creation of the Task */
	}
	if(curr_tcb_ptr == NULL){
		tcbs[task_idx].next_tcp_ptr = NULL;
		tcbs[task_idx].prev_tcp_ptr = NULL;
		tcbs[task_idx].priority = u8Priority;
		tcbs[task_idx].stack_save_ptr = &tcb_stack[task_idx][STD_STACK_SIZE - 16];
		tcbs[task_idx].state = READY;
		tcbs[task_idx].task = task;
		curr_tcb_ptr = &tcbs[task_idx];
		tcb_stack[task_idx][STD_STACK_SIZE - 1] = 0x01000000;
		tcb_stack[task_idx][STD_STACK_SIZE - 2] = (u32)task;

	}
	else{
		tcbs[task_idx].next_tcp_ptr = &tcbs[0];			/* circular linked-list */
		tcbs[task_idx - 1].next_tcp_ptr = &tcbs[task_idx];
		tcbs[task_idx].prev_tcp_ptr = &tcbs[task_idx - 1];
		tcbs[task_idx].priority = u8Priority;
		tcbs[task_idx].stack_save_ptr = &tcb_stack[task_idx][STD_STACK_SIZE - 16];
		tcbs[task_idx].state = READY;
		tcbs[task_idx].task = task;
		tcb_stack[task_idx][STD_STACK_SIZE - 1] = 0x01000000;
		tcb_stack[task_idx][STD_STACK_SIZE - 2] = (u32)task;
	}

	task_idx++;
}

/* this function is called at the systick tick interrupt to amnage the tasks and update curr_tcb_ptr to point to
 * next task's tcb block before context switching */
void Bartos_manageTasks(void){
	GPIO_TogglePin('G', P14);
	GPIO_TogglePin('G', P13);
	if(curr_tcb_ptr->next_tcp_ptr != NULL){
		curr_tcb_ptr = curr_tcb_ptr->next_tcp_ptr;
	}
	else{
		/* No other Task is available */
	}

}

void Bartos_start(void){
	if(curr_tcb_ptr != NULL){
		STK_voidInit();
		STK_SetPeriodicTicks(1000000);		/* 1 sec tick */
		LaunchScheduler();
	}
	else{
		/* No tasks are available to run */
	}
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

    __asm("CPSIE   I ");		/* Enable interrupts */
    __asm("BX      LR");		/* go to our first Task */
}

__attribute__((naked))
void SysTick_Handler(void){
	/* STEP 1 - SAVE THE CURRENT TASK CONTEXT

	At this point the processor has already pushed PSR, PC, LR, R12, R3, R2, R1 and R0
	onto the stack. We need to push the rest(i.e R4, R5, R6, R7, R8, R9, R10 & R11) to save the
	context of the current task. */

    __asm("CPSID		I");				/* Disable interrupts */
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

    /* enable interrupts */
	__asm("CPSIE   I");

    /* Return from interrupt */
    __asm("LDR	   R0, =curr_return_address");
    __asm("LDR     LR, [R0]");
	__asm("BX      LR");
    /* what happens after returning from this exception (interrupt) is that the processor pop R0-R3, R12, LR, PC and PSR
     from the current pointed stack (we edited it to point to next task's stack) into their original registers
     PC is also popped among the context so the processor goes to the location of next task pointed by it's PC it just popped
     we already popped R4-R11. with the automatically popped context (R0-R3, R12, LR, PSR),
     the context is now complete upon task execution */
}
