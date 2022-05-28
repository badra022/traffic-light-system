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

#include "systick.h"
#include "systick_private.h"

/* options: AHB_DIV_8, AHB */
#define CLOCK_SOURCE				AHB_DIV_8

static FUNCTION_PTR g_StkCallBackFunction = NULL;

/************************************************************/
/*						FUNCTION DEFINITIONS				*/
/************************************************************/

static void STK_voidSetCallBackFunction(FUNCTION_PTR function)
{
	g_StkCallBackFunction = function;
}

void STK_voidInit(void)
{
	/* choose the clock and enable the peripheral's interrupt */
	SysTick->CTRL = CLOCK_SOURCE | 0x02;
	
}
void STK_voidDelay(u32 u32Ticks)
{
	/* set the load register */
	if(u32Ticks < 0xFFFFFF)
	{
		SysTick->LOAD = u32Ticks;
	}
	/* start the count down (using or not using interrupt)*/
	SysTick->CTRL |= 0x01;
	while(!(SysTick->CTRL >> COUNTFLAG_BIT)){}
	return;
}

void STK_SetPeriodicFunction(u32 u32Ticks, FUNCTION_PTR function)
{
		/* set the load register */
	if(u32Ticks < 0xFFFFFF)
	{
		SysTick->LOAD = u32Ticks - 1;
	}
	/* set the call back function in the ISR */
	STK_voidSetCallBackFunction(function);
	
	/* start the count down (using or not using interrupt)*/
	SysTick->CTRL |= 0x01;
	
}

void STK_SetPeriodicTicks(u32 u32Ticks)
{
	/* set the load register */
	if(u32Ticks < 0xFFFFFF)
	{
		SysTick->LOAD = u32Ticks - 1;
	}

	/* start the count down */
	SysTick->CTRL |= 0x01;
}

void STK_voidStopTimer(void)
{
	SysTick->CTRL &= ~1;	
}

u32  STK_u32GetRemainingTime(void)
{
	return SysTick->VALUE;
}

u32  STK_u32GetElapsedTime(void)
{
	return ((u32)SysTick->LOAD - (u32)SysTick->VALUE);
}

//void SysTick_Handler(void)
//{
//	if(g_StkCallBackFunction != NULL)
//	{
//		g_StkCallBackFunction();
//	}
//}


