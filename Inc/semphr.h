/*
 * smphr.h
 *
 *  Created on: Jun 3, 2022
 *      Author: Badra
 */

#ifndef INC_SEMPHR_H_
#define INC_SEMPHR_H_

typedef struct binary_semaphore{
	u8 count;
	u8 maxCount;
	tcb_dtype* tcbBlockQueue;
}semphr_dtype;

typedef semphr_dtype* semphrHandler_dtype;


semphrHandler_dtype BARTOS_createBinarySemaphore(void);
semphrHandler_dtype BARTOS_createCountingSemaphore(u8 count, u8 maxCount);
u8 BARTOS_semaphoreGet(semphrHandler_dtype handle, u32 timeout);
u8 BARTOS_semaphorePut(semphrHandler_dtype handle);





#endif /* INC_SEMPHR_H_ */
