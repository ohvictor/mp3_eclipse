/***************************************************************************//**
  @file     circular_queue.h
  @brief
  @author
 ******************************************************************************/


#ifndef _CIRCULAR_QUEUE_H_
#define _CIRCULAR_QUEUE_H_


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define 	QUEUE_MAX_SIZE 		100

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct{
	size_t 		dataSize;					//size of an element
	size_t		bufferSize;					//size of the array
	uint8_t*	buffer;						//array of the queue
	uint16_t 	front;
	uint16_t 	rear;
} circular_queue_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief
 * @param
 * @return
 */
circular_queue_t createQueue(void* buffer, size_t bufferSize, size_t dataSize);

/**
 * @brief
 * @param
 * @return
 */
bool isEmpty(circular_queue_t* queue);

/**
 * @brief
 * @param
 * @return
 */
bool isFull(circular_queue_t* queue);

/**
 * @brief
 * @param
 * @return
 */
void clearQueue(circular_queue_t* queue);


/**
 * @brief
 * @param
 * @return
 */
size_t usageQueue(circular_queue_t* queue);


/**
 * @brief
 * @param
 * @return
 */
bool enqueue(circular_queue_t* queue, void* data);

/**
 * @brief
 * @param
 * @return
 */
bool enqueueChain(circular_queue_t* queue, const void* data, size_t length);

/**
 * @brief
 * @param
 * @return
 */
void* dequeue(circular_queue_t* queue);

/**
 * @brief
 * @param
 * @return
 */
void dequeueChain(circular_queue_t* queue, void* destination, size_t length);

/*******************************************************************************
 ******************************************************************************/

#endif // _CIRCULAR_QUEUE_H_
