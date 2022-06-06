/*
 * messegequeue.h
 *
 *  Created on: Jun 5, 2022
 *      Author: Badra
 */

#ifndef INC_MSGQUEUE_H_
#define INC_MSGQUEUE_H_

#include "queue.h"

typedef struct messegeQueue{
	tcb_dtype* receiverBlockingQueue;
	tcb_dtype* senderBlockingQueue;
	QueueHandler_dtype dataQueue;
}msgQueue_dtype;

typedef msgQueue_dtype* msgQueueHandler_dtype;

msgQueueHandler_dtype BARTOS_createQueue(u8* container, u32 capacity);
u8 BARTOS_QueueGet(msgQueueHandler_dtype handle, u32 timeout, u8* return_ptr);
u8 BARTOS_QueuePut(msgQueueHandler_dtype handle, u32 timeout, u8 data);


#endif /* INC_MSGQUEUE_H_ */
