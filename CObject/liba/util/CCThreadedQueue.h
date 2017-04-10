/*
 * Copyright 2017 Brendan Bruner
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
 * March 22, 2017
 */
/**
 * @file
 */
#ifndef UTIL_CCTHREADEDQUEUE_H_
#define UTIL_CCTHREADEDQUEUE_H_

#include <Class.h>
#include <CError.h>
#include <CIQueue.h>
#include <COS.h>

#ifdef __unix__
#include <pthread.h>
#else
#error "Unsupported operating system. Do not use CCThreadedQueue.h/c"
#endif


/************************************************************************/
/* Error codes.								*/
/************************************************************************/
/**
 * @enum CCTQueueError
 * @ingroup Queues
 * @var CCTQUEUE_OK
 *	No error.
 * @var CCTQUEUE_ERR_OVERFLOW
 *	Queue is full, element not inserted.
 * @var CCTQUEUE_ERR_UNDERFLOW
 *	Queue is empty, nothing popped/peeked.
 * @var CCTQUEUE_ERR_TIMEOUT
 * 	Timeout waiting for conditions to insert/pop from queue.
 */
typedef enum
{
	/* Enum values must have one to one mapping with values of
	 * CIQueueError. Add error codes with increasing values, but
	 * do not change existing values.
	 */
	CCTQUEUE_OK = 0,
	CCTQUEUE_ERR_OVERFLOW = 1,
	CCTQUEUE_ERR_UNDERFLOW = 2,
	CCTQUEUE_ERR_TIMEOUT = 3
} CCTQueueError;


/************************************************************************/
/* Declare class and vtable.						*/
/************************************************************************/
/**
 * @struct CCThreadedQueue
 * @ingroup Queues
 * @brief
 *	Thread safe wrapper for queues derived from the CIQueue interface.
 * @details
 *	Thread safe wrapper for queues derived from the CIQueue interface. This
 *	class will use a pointer to strongly aggregate a CIQueue object and wrap it's
 *	methods to make them threadsafe. To that effect, it requires an already
 *	constructed queue at construction time.
 *	Destroying the CCThreadedQueue object will result in the destruction of the
 *	aggregate CIQueue object.
 *	@code{.c}
 *		struct CCThreadedQueue queue;
 *		struct CCArrayQueue queueBackbone;
 *
 *		CCArrayQueue(&queueBackbone, sizeof(char), 3);
 *		CCThreadedQueue(&queue, &queueBackbone->cIQueue);
 *
 *		char test = 'h'
 *		CCThreadedQueue_Insert(&queue, &test, COS_BLOCK_FOREVER);
 *
 *		char test_peek;
 *		CCThreadedQueue_Peek(&queue, &test_peek, COS_BLOCK_FOREVER);
 *		if( test_peek == 'h' ) {
 *			printf("success!");
 *		}
 *		
 *		CDestroy(&queue);
 *	@endcode
 * @attention
 *	Destroying a CCThreadQueue object will result in the destruction of the
 *	aggregate CIQueue object. See constructor, CCThreadedQueue(), for more details.
 * @attention
 * 	This class does not have any virtual methods. All class methods are not virtual.
 */
struct CCThreadedQueue
{
	/* Super class must always be first member */
	/* of a class' struct. */
	struct CObject cobject;

	struct     
	{	 
		struct CIQueue* backbone;

		/* Three OS objects are needed for thread safety.
		 * 		-> mutex
		 * 			Synchronize access to all backbone queue functions.
		 * 		-> insertBlock
		 * 			Provides a place for calling threads to block when they
		 * 			insert data, but the backbone is full.
		 * 		-> removeBlock
		 * 			Provides a place for calling threads to block when they
		 * 			read/remove data, but the backbone is empty.
		 */
#ifdef __unix__
		pthread_cond_t insertCondition;
		pthread_cond_t removeCondition;
		pthread_mutex_t mutex;
#endif

	} _;
};

/**
 * @ingroup VTable
 * @brief
 *	 Virtual table for struct CCThreadedQueue.
 */
struct CCThreadedQueue_VTable
{
	/* Space for a copy of the super class' virtual table must  */
	/* be the first member of a class virtual table declaration. */
	struct CObject_VTable  cobject_override;

	/* Since we are overriding the destructor, we need to keep */
	/* keep a reference to the super class' implementation of */
	/* the destructor. */
	const struct CObject_VTable* cobject;
};

/**
 * @memberof CCThreadedQueue
 * @ingroup VTable
 * @details
 *	Get reference to the struct CCThreadedQueue's vtable.
 */
const struct CCThreadedQueue_VTable* CCThreadedQueue_VTable_Key( );


/************************************************************************/
/* Constructor															*/
/************************************************************************/
/**
 * @memberof CCThreadedQueue
 * @constructor
 * @details
 *
 * @param self
 *	The instance of struct CCThreadedQueue to construct.
 * @param backbone
 * 	An instance of a CIQueue object. This will be used as the backbone
 * 	for all queue operations. Note, when the destructor is called, this
 * 	object will also be destroyed.
 * @returns
 *	CIQUEUE_OK on successful construction.
 */
CError CCThreadedQueue( struct CCThreadedQueue* self, struct CIQueue* backbone );


/************************************************************************/
/* Class methods. 							*/
/************************************************************************/
/**
 * @memberof CCThreadedQueue
 * @details
 *	Insert an element into the tail of the queue by copy.
 * @param self
 *	The queue.
 * @param element
 *	Pointer to data to copy into queue.
 * @param blockTime
 * 	Time in ms to wait for space in the queue.
 * @returns
 *	Error code.
 */
CCTQueueError CCThreadedQueue_Insert( struct CCThreadedQueue* self, const void* element, COS_Timemsec blockTime );

/**
 * @memberof CCThreadedQueue
 * @details
 *	Remove an element from the head of the queue by copy.
 * @param self
 *	The queue
 * @param element
 *	Data in the head of the queue will be copied into the location pointed to
 *	by this. Pass in NULL to remove the the head of the queue without copying
 *	out the data.
 * @param blockTime
 * 	Time in ms to wait for data to be in the queue.
 * @returns
 *	An error code.
 */
CCTQueueError CCThreadedQueue_Remove( struct CCThreadedQueue* self, void* element, COS_Timemsec blockTime );

/**
 * @memberof CCThreadedQueue
 * @details
 *	Peek at the element in the head of the queue by copy. This does not
 *	remove the head.
 * @param self
 *	The queue.
 * @param element
 *	Data in the head of the queue will be copied to the location pointed to
 *	by this.
 * @param blockTime
 *  Time to wait in ms for data to be in the queue.
 * @returns
 *	An error code.
 */
CCTQueueError CCThreadedQueue_Peek( struct CCThreadedQueue* self, void* element, COS_Timemsec blockTime );

/**
 * @memberof CIQueue
 * @details
 *	Get the number of elements currently in the queue. This is an unreliable method and should
 *	only be used for logging purposes.
 * @param self
 *	The queue.
 * @returns
 *	Number of elements currently in the queue.
 */
size_t CCThreadedQueue_Size( struct CCThreadedQueue* self );

/**
 * @memberof CCThreadedQueue
 * @details
 *	Get the maximum number of elements allowed in the queue.
 * @param self
 *	The queue.
 * @returns
 *	The maximum number of elements allowed in the queue.
 */
size_t CCThreadedQueue_MaxSize( struct CCThreadedQueue* self );

/*
 * TODO: clear method should allow user to enter a clean up function.
 */
/**
 * @memberof CCThreadedQueue
 * @details
 *	Reset the queue to having zero elements in it. All data
 *	copied into the queue is lost.
 * @param self
 *	The queue.
 */
void CCThreadedQueue_Clear( struct CCThreadedQueue* self );


#endif /* UTIL_CCTHREADEDQUEUE_H_ */
