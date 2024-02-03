/***************************************************************************//**
  @file     circular_queue.c
  @brief
  @author
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "circular_queue.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief
 * @param
 * @return
 */
circular_queue_t createQueue(void* buffer, size_t bufferSize, size_t dataSize)
{
	circular_queue_t queue = {
		.dataSize = dataSize,
		.bufferSize = bufferSize,
		.buffer = buffer,
		.front = 1,
		.rear = 0
	};
	return queue;
}

/**
 * @brief
 * @param
 * @return
 */
bool isEmpty(circular_queue_t* queue)
{
	return usageQueue(queue) == 0;
}

/**
 * @brief
 * @param
 * @return
 */
bool isFull(circular_queue_t* queue)
{
	return usageQueue(queue) == queue->bufferSize;
}

/**
 * @brief
 * @param
 * @return
 */
void clearQueue(circular_queue_t* queue)
{
	queue->front = 1;
	queue->rear = 0;
}


/**
 * @brief
 * @param
 * @return
 */
size_t usageQueue(circular_queue_t* queue)
{
	return (queue->bufferSize + queue->rear - queue->front + 1) % queue->bufferSize;
}


/**
 * @brief
 * @param
 * @return
 */
bool enqueue(circular_queue_t* queue, void* data)
{
	if (isFull(queue))
	{
		return false;
	}
	else
	{
		queue->rear = (queue->rear + 1) % queue->bufferSize;
		memcpy(queue->buffer + queue->rear * queue->dataSize, data, queue->dataSize);
		return true;
	}
}

/**
 * @brief
 * @param
 * @return
 */
bool enqueueChain(circular_queue_t* queue, const void* data, size_t length)
{
	if (usageQueue(queue) + length > queue->bufferSize)
	{
		return false;
	}
	else
	{
		for (size_t i = 0; i < length; i++)
		{
			enqueue(queue, data + i * queue->dataSize);
		}
		return true;
	}
}

/**
 * @brief
 * @param
 * @return
 */
void* dequeue(circular_queue_t* queue)
{
	if (isEmpty(queue))
	{
		return NULL;
	}
	else
	{
		void* data = queue->buffer + queue->front * queue->dataSize;
		queue->front = (queue->front + 1) % queue->bufferSize;
		return data;
	}
}

/**
 * @brief
 * @param
 * @return
 */
void dequeueChain(circular_queue_t* queue, void* destination, size_t length)
{
	if (usageQueue(queue) < length)
	{
		return;
	}
	else
	{
		for (size_t i = 0; i < length; i++)
		{
			memcpy(destination + i * queue->dataSize, dequeue(queue), queue->dataSize);
		}
	}
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/******************************************************************************/
