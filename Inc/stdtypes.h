/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 30/8/2020				*************/
/*********** Version	: V01					*************/
/************************************************************/

/************************************************************/
/*			GUARD FILE TO PREVENT MULTIPLE INCLUSION		*/
/************************************************************/
#ifndef STD_TYPES_H_
#define STD_TYPES_H_

/************************************************************/
/*					USER DEFINED DATATYPES					*/
/************************************************************/
/* Boolean Data Type */
typedef unsigned char bool;

typedef unsigned char         u8;          /*           0 .. 255             */
typedef signed char           s8;          /*        -128 .. +127            */
typedef unsigned short        u16;         /*           0 .. 65535           */
typedef signed short          s16;         /*      -32768 .. +32767          */
typedef unsigned long         u32;         /*           0 .. 4294967295      */
typedef signed long           s32;         /* -2147483648 .. +2147483647     */
typedef unsigned long long    u64;         /*       0..18446744073709551615  */
typedef signed long long      s64;
typedef float                 float32;
typedef double                float64;

/************************************************************/
/*									MACROS					*/
/************************************************************/
#ifndef DISABLE
#define DISABLE       (0u)
#endif

#ifndef ENABLE
#define ENABLE        (1u)
#endif

#define OK		0
#define ERROR	1
#define ERR_INVALID_PARAMETER		2
#define ERR_INVALID_ARR_INDEX				15
#define ERR_ENQUEUE					20
#define ERR_DEQUEUE					21
#define ERR_EMPTY_QUEUE				22
#define ERR_FAILED_TO_PERFORM		50


#define MAX_UNSIGNED_CHARACTER		(u8)(~0)
#define MAX_UNSIGNED_SHORT			(u16)(~0)
#define MAX_UNSIGNED_LONG			(u32)(~0)

#ifndef FALSE
#define FALSE       (0u)
#endif

#ifndef TRUE
#define TRUE        (1u)
#endif

#define HIGH        (1u)
#define LOW         (0u)

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef void (* FUNCTION_PTR) (void);
typedef void (* FUNCTION_PTR_VOID_ARGS) (void*);

#define INT_MAX		(u32)4000000000

#define	P0		0
#define	P1		1
#define	P2		2
#define	P3		3
#define	P4		4
#define	P5		5
#define	P6		6
#define	P7		7
#define	P8		8
#define	P9		9
#define	P10		10
#define	P11		11
#define	P12		12
#define	P13		13
#define	P14		14
#define	P15		15

typedef struct function_dtype
{
	FUNCTION_PTR function;		/* pointer to the function */
	void*		args;			/* pointer to function arguments passed to it*/
}function_dtype;


#endif /* STD_TYPE_H_ */
