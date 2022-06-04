/*
 * smphr.h
 *
 *  Created on: Jun 3, 2022
 *      Author: Badra
 */

#ifndef INC_BINARYSEMPHR_H_
#define INC_BINARYSEMPHR_H_

typedef struct binary_semaphore{
	u8 state;
	tcb_dtype* tcbBlockQueue;
}binarySemphr_dtype;

typedef binarySemphr_dtype* binarySemphrHandle_dtype;


binarySemphrHandle_dtype BARTOS_createBinarySemaphore(void);
u8 BARTOS_semaphoreGet(binarySemphrHandle_dtype handle, u32 timeout);
u8 BARTOS_semaphorePut(binarySemphrHandle_dtype handle);





#endif /* INC_BINARYSEMPHR_H_ */
