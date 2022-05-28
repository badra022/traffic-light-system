/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 30/8/2020				*************/
/*********** Version	: V02					*************/
/************************************************************/


/************************************************************/
/*			GUARD FILE TO PREVENT MULTIPLE INCLUSION		*/
/************************************************************/
#ifndef _SYSTICK_PRIVATE_H_
#define _SYSTICK_PRIVATE_H_

typedef struct
{
  u32 CTRL;                    /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
  u32 LOAD;                    /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register       */
  u32 VALUE;                     /*!< Offset: 0x008 (R/W)  SysTick Current Value Register      */
  u32 CALIB;                   /*!< Offset: 0x00C (R/ )  SysTick Calibration Register        */
} SysTick_Type;

#define SCS_BASE            (0xE000E000UL) /*!< System Control Space Base Address  */
#define SysTick_BASE        (SCS_BASE +  0x0010UL)
#define SysTick             ((SysTick_Type   *)     SysTick_BASE  )   /*!< SysTick configuration struct       */

#define AHB_DIV_8		0
#define AHB				4

#define COUNTFLAG_BIT	16

#endif
