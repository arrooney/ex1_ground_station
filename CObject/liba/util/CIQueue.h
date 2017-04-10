/*
 * Copyright 2015 Brendan Bruner
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * bbruner@ualberta.ca
 * Jun 24, 2016
 */
/**
 * @file
 * @defgroup Queues
 * All queue type data structures.
 */
#ifndef UTIL_CIQUEUE_H_
#define UTIL_CIQUEUE_H_

#include <Class.h>


/************************************************************************/
/* Error codes.								*/
/************************************************************************/
/**
 * @enum CIQueueError
 * @ingroup Queues
 * @var CIQUEUE_OK
 *	No error.
 * @var CIQUEUE_ERR_OVERFLOW
 *	Queue is full, element not inserted.
 * @var CIQUEUE_ERR_UNDERFLOW
 *	Queue is empty, nothing popped/peeked.
 */
typedef enum
{
	CIQUEUE_OK = 0,
	CIQUEUE_ERR_OVERFLOW = 1,
	CIQUEUE_ERR_UNDERFLOW = 2
} CIQueueError;


/************************************************************************/
/* Declare interface and vtable.					*/
/************************************************************************/
/**
 * @struct CIQueue
 * @ingroup Queues
 * @brief
 *	Queue data structure interface.
 * @details
 *	Interface for queues. Provides methods for inserting, popping,
 *	peeking, and polling the queue's size. 
 *	<br>Implementing classes must implement the behaviour specified in the
 *	functions described below.
 *	<br>This is a copy by value queue. That means a pointer to data is
 *	is given to the queues functions, and the data pointed to is then copied
 *	into the queue.
 *	<br>Implementing classes must provide a way to define the length of data
 *	copied in/out of the queue.
 */
struct CIQueue
{
	/* CInterface must always be first member of */
	/* an interfaces struct .*/
	struct CInterface interface;
};

/**
 * @ingroup VTable
 * @brief
 *	CIQueue virtual table.
 */
struct CIQueue_VTable
{
	CIQueueError (*insert)( struct CIQueue*, const void* );
	CIQueueError (*remove)( struct CIQueue*, void* );
	CIQueueError (*peek)( struct CIQueue*, void* );
	size_t (*size)( struct CIQueue* );
	size_t (*maxSize)( struct CIQueue* );
	void (*clear)( struct CIQueue* );
};


/************************************************************************/
/* Class methods. 							*/
/************************************************************************/
/**
 * @memberof CIQueue
 * @details
 *	Insert an element into the tail of the queue by copy.
 * @param self
 *	The queue.
 * @param element
 *	Pointer to data to copy into queue.
 * @returns
 *	Error code.
 */
CIQueueError CIQueue_Insert( struct CIQueue* self, const void* element );

/**
 * @memberof CIQueue
 * @details
 *	Remove an element from the head of the queue by copy.
 * @param self
 *	The queue
 * @param element
 *	Data in the head of the queue will be copied into the location pointed to
 *	by this. Pass in NULL to remove the the head of the queue without copying
 *	out the data.
 * @returns
 *	An error code.    
 */
CIQueueError CIQueue_Remove( struct CIQueue* self, void* element );

/**
 * @memberof CIQueue
 * @details
 *	Peek at the element in the head of the queue by copy. This does not
 *	remove the head.
 * @param self
 *	The queue.
 * @param element
 *	Data in the head of the queue will be copied to the location pointed to
 *	by this.
 * @returns
 *	An error code.
 */
CIQueueError CIQueue_Peek( struct CIQueue* self, void* element );

/**
 * @memberof CIQueue
 * @details
 *	Get the number of elements currently in the queue.
 * @param self
 *	The queue.
 * @returns
 *	Number of elements currently in the queue. 
 */
size_t CIQueue_Size( struct CIQueue* self );

/**
 * @memberof CIQueue
 * @details
 *	Get the maximum number of elements allowed in the queue.
 * @param self
 *	The queue.
 * @returns
 *	The maximum number of elements allowed in the queue. 
 */	
size_t CIQueue_MaxSize( struct CIQueue* self );

/*
 * TODO: clear method should allow user to enter a clean up function.
 */
/**
 * @memberof CIQueue
 * @details
 *	Reset the queue to having zero elements in it. All data
 *	copied into the queue is lost.
 * @param self
 *	The queue.
 */
void CIQueue_Clear( struct CIQueue* self );


#endif /* UTIL_CIQUEUE_H_ */
