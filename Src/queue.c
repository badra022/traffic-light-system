/*
 * queue.c
 *
 *  Created on: May 27, 2022
 *      Author: Badra
 */

#include <stdtypes.h>
#include <stdlib.h>
#include "MATH_macros.h"

#include "queue.h"

Queue_dtype* createQueue(u32* arr, u32 capacity){
	Queue_dtype* queue_ptr = (Queue_dtype*)malloc(sizeof(Queue_dtype));
	if(queue_ptr != NULL){
		queue_ptr->array = arr;
		queue_ptr->capacity = capacity;
		queue_ptr->front = 0;
		queue_ptr->rear = queue_ptr->capacity - 1;		/* to make it be 0 at the first enqueue */
		queue_ptr->size = 0;
	}
	else{
		/* Memory not allocated. */
	}
	return queue_ptr;
}

void deleteQueue(Queue_dtype* queue_ptr){
	free(queue_ptr);
}

u8 isQueueFull(Queue_dtype* queue_ptr){
	return (queue_ptr->size == queue_ptr->capacity);
}

u8 isQueueEmpty(Queue_dtype* queue_ptr){
	return (queue_ptr->size == 0);
}

void enqueue(Queue_dtype* queue_ptr, u32 item){
	if(isQueueFull(queue_ptr)){
		return;
	}
	else{
		queue_ptr->array[queue_ptr->rear] = item;
		queue_ptr->rear = (queue_ptr->rear + 1) % queue_ptr->capacity;
		queue_ptr->size += 1;
	}
}

u32 dequeue(Queue_dtype* queue_ptr){
	if(isQueueEmpty(queue_ptr)){
		return INT_MAX;
	}
	else{
		u32 item = queue_ptr->array[queue_ptr->front];
		queue_ptr->front = (queue_ptr->front + 1) % queue_ptr->capacity;
		queue_ptr->size -= 1;
		return item;
	}
}

u32 getQueueFront(Queue_dtype* queue_ptr){
	return queue_ptr->front;
}

u32 getQueueRear(Queue_dtype* queue_ptr){
	return queue_ptr->rear;
}

u32 getQueueSize(Queue_dtype* queue_ptr){
	return queue_ptr->size;
}
