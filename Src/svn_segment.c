/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 2/1/2021				*************/
/*********** Version	: V01					*************/
/************************************************************/

#include "stdtypes.h"
#include "macros.h"
#include "port.h"

#include "gpio.h"
#include "svn_segment.h"


#define SEVEN_SEGMENT_1_ENABLE			P8
#define SEVEN_SEGMENT_2_ENABLE			P9


u8 curr_port;
static u8 numbers[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void svn_segment_write(u8 number){
  for(u8 i = 0; i < 7; i++){
	  GPIO_WritePin(curr_port, i, (numbers[number] >> i) & 0x01);
  }
}

void svn_segment_1_enable(void){
	GPIO_WritePin(curr_port, SEVEN_SEGMENT_1_ENABLE, TRUE);
}

void svn_segment_1_disable(void){
	GPIO_WritePin(curr_port, SEVEN_SEGMENT_1_ENABLE, FALSE);
}

void svn_segment_2_enable(void){
	GPIO_WritePin(curr_port, SEVEN_SEGMENT_2_ENABLE, TRUE);
}

void svn_segment_2_disable(void){
	GPIO_WritePin(curr_port, SEVEN_SEGMENT_2_ENABLE, FALSE);
}

void svn_segment_init(u8 port){
	curr_port = port;
	GPIO_Init(curr_port, SEVEN_SEGMENT_1_ENABLE, OUTPUT, PUSH_PULL, NO_PULLING);
	GPIO_Init(curr_port, SEVEN_SEGMENT_2_ENABLE, OUTPUT, PUSH_PULL, NO_PULLING);
	for(unsigned char i = 0; i < 7; i++){
		GPIO_Init(curr_port, i , OUTPUT, PUSH_PULL, NO_PULLING);
	}
}
