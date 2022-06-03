/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 24/11/2020			*************/
/*********** Version	: V01					*************/
/************************************************************/

#ifndef INC_GPIO_H_
#define INC_GPIO_H_


typedef enum{
	AF0,
	AF1,
	AF2,
	AF3,
	AF4,
	AF5,
	AF6,
	AF7,
	AF8,
	AF9,
	AF10,
	AF11,
	AF12,
	AF13,
}AF_dtype;

//write result
#ifndef OK
#define OK  ((unsigned char)0)
#endif
#define NOK ((unsigned char)1)

//mode types
#define INPUT ((unsigned int)0x00)
#define OUTPUT ((unsigned int)0x01)
#define ALTERNATE_FUN ((unsigned int)0x02)
#define ANALOG ((unsigned int)0x03)

//output modes
#define PUSH_PULL ((unsigned int)0x00)
#define OPEN_DRAIN ((unsigned int)0x01)

#define PULL_UP		((unsigned char)0x01)
#define PULL_DOWN	((unsigned char)0x02)
#define NO_PULLING	((unsigned char)0x00)

void GPIO_Init(char port_id, unsigned int PIN_NO, unsigned int PIN_Dir, unsigned int Default_State, unsigned char Pulling_State);
unsigned char GPIO_WritePin(char port_id, unsigned int PIN_NO, unsigned int Data);
unsigned char GPIO_ReadPin(char port_id, unsigned int PIN_NO);
void GPIO_setAlternateFunction(char port_id, unsigned int PIN_NO, AF_dtype selected_function);
unsigned char GPIO_TogglePin(char port_id, unsigned int PIN_NO);



#endif /* INC_GPIO_H_ */
