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
 */
#ifndef UTIL_CCARRAYQUEUE_H_
#define UTIL_CCARRAYQUEUE_H_

#include <Class.h>
#include <CError.h>
#include "CIQueue.h"


/************************************************************************/
/* Declare class and vtable.						*/
/************************************************************************/
/**
 * @struct CCArrayQueue
 * @extends CIQueue
 * @ingroup Queues
 * @brief
 *	Array implementation of CIQueue interface.
 * @details
 *	Implements the struct CIQueue interface using an array as the
 *	underlying data structure. This is a copy by value queue. 
 *	Below is an example of creating a new queue of size three to
 *	hold chars, then, adding an element and peeking at the added element.
 *	Note, error checking should be done, but is ommitted in this example.
 *	@code{.c}
 *		struct CCArrayQueue queue;
 *		CCArrayQueue(&queue, sizeof(char), 3);
 *
 *		char test = 'h'
 *		CIQueue_Insert(&queue.cIQueue, &test);
 *
 *		char test_peek;
 *		CIQueue_Peek(&queue.cIQueue, &test_peek);
 *		if( test_peek == 'h' ) {
 *			printf("success!");
 *		}
 *		
 *		CDestroy(&queue);
 *	@endcode
 * @attention
 *	All functions inherited from the CIQueue interface can be called
 *	using the CCArrayQueue::cIQueue as their first argument. 
 */
struct CCArrayQueue
{
	/* Super class must always be first member */
	/* of a class' struct. */
	struct CObject cobject;

	/* Implement the CIQueue interface. */
	struct CIQueue ciqueue;

	/* Private member variables. */
	struct     
	{
		size_t size;
		size_t max_size;
		size_t element_size;
		size_t head;
		size_t tail;
		unsigned char is_static;
		unsigned char* queueBase;
	} _;
};

/**
 * @ingroup VTable
 * @brief
 *	 Virtual table for struct CCArrayQueue.
 */
struct CCArrayQueue_VTable
{
	/* Space for a copy of the super class' virtual table must  */
	/* be the first member of a class virtual table declaration. */
	struct CObject_VTable cobject_override;

	/* Since we are overriding the destructor, we need to keep */
	/* keep a reference to the super class' implementation of */
	/* the destructor. */
	const struct CObject_VTable* cobject;

	/* Space for a copy of the implemented interface's virtual table */
	struct CIQueue_VTable ciqueue_override;
};

/**
 * @memberof CCArrayQueue
 * @ingroup VTable
 * @details
 *	Get reference to the struct CCArrayQueue's vtable.
 */
const struct CCArrayQueue_VTable* CCArrayQueue_GetVTable( );


/************************************************************************/
/* Constructor								*/
/************************************************************************/
/**
 * @memberof CCArrayQueue
 * @constructor
 * @sa
 *	- CCArrayQueueStatic
 * @details
 *	Constructor for class struct CCArrayQueue. This method uses
 *	CMalloc defined in Class.h to allocate space for the queue.
 *	Calling the destructor, CDestructor( ), will free this memory with
 *	the free method, CFree, defined in Class.h.
 * @param self
 *	The instance of struct CCArrayQueue to construct.
 * @param element_size
 *	The size of an element in the queue in bytes.
 * @param queue_size
 *	The maximum number of elements which can be put in the queue.
 * @returns
 *	CIQUEUE_OK on successful construction.
 */
CError CCArrayQueue( struct CCArrayQueue* self, size_t element_size, size_t queue_size);

/**
 * @memberof CCArrayQueue
 * @constructor
 * @details
 *	Constructor for class struct CCArrayQueue. Allows application code to
 *	define memory region to use for the queue's array back end.
 * @param self
 *	The instance of struct CCArrayQueue to construct.
 * @param memory
 *	A pointer to memory to use for the queue. The memory pointed
 *	to must be at least element_size * queue_size bytes long.
 * @param element_size
 *	The size of an element in the queue in bytes.
 * @param queue_size
 *	The maximum number of elements which can be put in the queue.
 * @returns
 *	CIQUEUE_OK on successful construction.
 */
CError CCArrayQueueStatic( struct CCArrayQueue* self, void* memory, size_t element_size, size_t queue_size );


#endif /* UTIL_CCARRAYQUEUE_H_ */
