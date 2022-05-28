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
	u32* array;
}Queue_dtype;

Queue_dtype* createQueue(u32* arr, u32 capacity);
void deleteQueue(Queue_dtype* queue_ptr);
u8 isQueueFull(Queue_dtype* queue_ptr);
u8 isQueueEmpty(Queue_dtype* queue_ptr);
void enqueue(Queue_dtype* queue_ptr, u32 item);
u32 dequeue(Queue_dtype* queue_ptr);
u32 getQueueFront(Queue_dtype* queue_ptr);
u32 getQueueRear(Queue_dtype* queue_ptr);
u32 getQueueSize(Queue_dtype* queue_ptr);

#endif /* INC_QUEUE_H_ */
