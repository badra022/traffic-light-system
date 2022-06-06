/*
 * queue.h
 *
 *  Created on: May 27, 2022
 *      Author: Badra
 */

#ifndef INC_QUEUE_H_
#define INC_QUEUE_H_

typedef struct Queue{
	u32 front;
	u32 rear;
	u32 size;
	u32 capacity;
	u8* array;
}Queue_dtype;

typedef Queue_dtype* QueueHandler_dtype;

QueueHandler_dtype createQueue(u8* arr, u32 capacity);
u8 deleteQueue(QueueHandler_dtype queue_ptr);
u8 isQueueFull(QueueHandler_dtype queue_ptr);
u8 isQueueEmpty(QueueHandler_dtype queue_ptr);
u8 enqueue(QueueHandler_dtype queue_ptr, u8 item);
u8 dequeue(QueueHandler_dtype queue_ptr, u8* return_ptr);
u8 getQueueFront(QueueHandler_dtype queue_ptr, u8* return_ptr);
u8 getQueueRear(QueueHandler_dtype queue_ptr, u8* return_ptr);
u8 getQueueSize(QueueHandler_dtype queue_ptr, u8* return_ptr);

#endif /* INC_QUEUE_H_ */
