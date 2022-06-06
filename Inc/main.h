/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 13/3/2022				*************/
/*********** Version	: V01					*************/
/************************************************************/
#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include "stdtypes.h"
#include "macros.h"

#include "port.h"
#include "bartos.h"
#include "gpio.h"
#include "gpt.h"
#include "nvic.h"
#include "rcc.h"
#include "uart.h"

/*****************************************************************************/
/***********************    APPLICATION APIs     *****************************/
/*****************************************************************************/
void APP_toggleLedTaskSetup(void);
void APP_toggleLed(void);
void APP_setupIOConfigs(void);

#endif /* INC_MAIN_H_ */
