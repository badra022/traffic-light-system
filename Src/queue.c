/*
 * queue.c
 *
 *  Created on: May 27, 2022
 *      Author: Badra
 */

#include "stdtypes.h"
#include "macros.h"
#include "port.h"
#include <stdlib.h>

#include "queue.h"

QueueHandler_dtype createQueue(u8* arr, u32 capacity){
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

u8 deleteQueue(QueueHandler_dtype queue_ptr){
	u8 status;
	if(queue_ptr == NULL){
		status = ERR_INVALID_PARAMETER;
	}
	else{
		free(queue_ptr);
		status = OK;
	}
	return status;
}

u8 isQueueFull(QueueHandler_dtype queue_ptr){
	return (queue_ptr->size == queue_ptr->capacity);
}

u8 isQueueEmpty(QueueHandler_dtype queue_ptr){
	return (queue_ptr->size == 0);
}

u8 enqueue(QueueHandler_dtype queue_ptr, u8 item){
	u8 status;
	if(isQueueFull(queue_ptr)){
		status = ERR_QUEUE;
	}
	else{
		queue_ptr->rear = (queue_ptr->rear + 1) % queue_ptr->capacity;
		queue_ptr->size += 1;
		queue_ptr->array[queue_ptr->rear] = item;
		status = OK;
	}
	return status;
}

u8 dequeue(QueueHandler_dtype queue_ptr, u8* return_ptr){
	u8 status;
	if(isQueueEmpty(queue_ptr)){
		status = ERR_QUEUE;
	}
	else{
		*return_ptr = queue_ptr->array[queue_ptr->front];
		queue_ptr->front = (queue_ptr->front + 1) % queue_ptr->capacity;
		queue_ptr->size -= 1;
		status = OK;
	}
	return status;
}

u8 getQueueFront(QueueHandler_dtype queue_ptr, u8* return_ptr){
	u8 status;
	if(queue_ptr == NULL){
		status = ERR_INVALID_PARAMETER;
	}
	else{
		*return_ptr = queue_ptr->front;
		status = OK;
	}
	return status;
}

u8 getQueueRear(QueueHandler_dtype queue_ptr, u8* return_ptr){
	u8 status;
	if(queue_ptr == NULL){
		status = ERR_INVALID_PARAMETER;
	}
	else{
		*return_ptr = queue_ptr->rear;
		status = OK;
	}
	return status;
}

u8 getQueueSize(QueueHandler_dtype queue_ptr, u8* return_ptr){
	u8 status;
	if(queue_ptr == NULL){
		status = ERR_INVALID_PARAMETER;
	}
	else{
		*return_ptr = queue_ptr->size;
		status = OK;
	}
	return status;
}
