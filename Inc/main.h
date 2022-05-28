/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 13/3/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/
#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include <stdtypes.h>
#include "MATH_macros.h"

#include "gpio.h"
#include "nvic.h"
#include "uart.h"
#include "gpt.h"
#include "rcc.h"
#include "systick.h"
#include "bartos.h"

/*****************************************************************************/
/***********************    APPLICATION APIs     *****************************/
/*****************************************************************************/
void APP_toggleLedTaskSetup(void);
void APP_toggleLed(void);
void APP_setupIOConfigs(void);

#endif /* INC_MAIN_H_ */
