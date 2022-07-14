/*
 * ADC.c
 *
 *  Created on: 5 JUNE, 2022
 *      Author: YAHYA OSAMA
 */

#include "stdtypes.h"
#include "macros.h"
#include "port.h"

#include "gpio.h"
#include "rcc.h"
#include "ADC.h"


void ADC_init()
{

	RCC_voidEnableClock(RCC_APB2, 8);			// EN ADC CLCK
	GPIO_Init('A', P5, ANALOG, PUSH_PULL, NO_PULLING);

	ADC_SR = 0x00;
	ADC_CR1 |= ( 0x10 << ADC_RES ); 				// RESOLUTION 8 BIT
	ADC_CR2 |= ( 0x01 << ADC_ADON ) | ( 0x00 << ADC_CONT ); 	// continous mode and enable adc
	ADC_CR2 |= ( 0x01 << ADC_EOCS ); 				// End of conversion selection flag
	ADC_SMPR2 |= ( 0x01 << 15 ) ; // channel5 sampling time is 15 APB2 cycle
	ADC_SQR3  |= ( 0x01 << 2 ) | ( 0x01 << 0 ); // choosing channel5 as first sequence

	ADC_CR2 |= ( 0x01 << ADC_ADON );       	// for opening adc
}




u16 ADC_GetValue(u8 channel)
{
	u16 value = 0;

	ADC_CR2 &= ~(1 << ADC_SWSTART);

	if(ADC_SR & (1 << ADC_OVR))
	{
		ADC_SR &= ~(1 << ADC_OVR);
	}


	ADC_SQR3 &= ~(0x1F << ADC_SQ1);
	ADC_SQR3 |=  (channel << ADC_SQ1);

	ADC_CR2 |=   (1 << ADC_SWSTART);

	while(!(ADC_SR  & (1 << ADC_EOC)));
	value = (u16)(ADC_DR);

	return value;
}




float32 ADC_GetVoltage(u8 channel)
{
	float32 voltage = 0;
	u16 ADC_val = ADC_GetValue(channel);
	voltage = ((float32)(ADC_REF_VOL * ADC_val) / (float32)255);		// for 8 bit resolution
	return voltage;
}

