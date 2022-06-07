/*
 * ADC.h
 *
 *  Created on: 5 JUNE, 2022
 *      Author: Yahya Osama
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_


void ADC_init();
float32 ADC_GetVoltage(u8 channel);
u16 ADC_GetValue(u8 channel);


#endif /* INC_ADC_H_ */
