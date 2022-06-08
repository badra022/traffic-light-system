/*
 ============================================================================
 Name        : PWM.h
 Author      : Ahmed Khaled Hilal
 Description : PWM driver header file
 ============================================================================
*/




#ifndef INC_PWM_H_
#define INC_PWM_H_

#define ARR_VALUE (1000)

void PWM_INIT(Gptim2_5_dtype* timer, u8 channel_no, u16 prescaler, u32 duty_cycle, u32 preload);
void PWM_ChangeDutycycle(Gptim2_5_dtype* timer, u8 channel_no, u8 duty_cycle);
void PWM_START_TIMER(Gptim2_5_dtype* timer);

#endif /* INC_PWM_H_ */
