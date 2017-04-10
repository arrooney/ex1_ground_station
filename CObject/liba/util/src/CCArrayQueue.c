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
 * Jun 25, 2016
 */

#include <CCArrayQueue.h>
#include <string.h>
#include <stdlib.h>


/************************************************************************/
/* Private methods							*/
/************************************************************************/
static inline void CCArrayQueue_IncrementTail( struct CCArrayQueue* self )
{
	CAssertObject(self);

	/* Move pointer to next element in queue. */
	self->_.tail = (self->_.tail + self->_.element_size) % (self->_.element_size * self->_.max_size);
}

static inline void CCArrayQueue_IncrementHead( struct CCArrayQueue* self )
{
	CAssertObject(self);

	/* Move pointer to next element in queue. */
	self->_.head = (self->_.head + self->_.element_size) % (self->_.element_size * self->_.max_size);
}

/************************************************************************/
/* Virtual methods							*/
/*************************************************************************/
static CIQueueError CIQueue_Insert_Def( struct CIQueue* self_, const void* element )
{
	struct CCArrayQueue* self = CCast(self_);

	/* If the queue is full, return an overflow error. */
	if( self->_.size == self->_.max_size ) {
		return CIQUEUE_ERR_OVERFLOW;
	}

	/* Queue is not full, insert element. */
	memcpy(&self->_.queueBase[self->_.head], element, self->_.element_size);

	/* Increment head to next queue element. */
	CCArrayQueue_IncrementHead(self);

	/* Increase current size of queue. */
	++self->_.size;

	return CIQUEUE_OK;
}

static CIQueueError CIQueue_Remove_Def( struct CIQueue* self_, void* element )
{
	struct CCArrayQueue* self = CCast(self_);

	/* Check if the queue is empty. */
	if( self->_.size == 0 ) {
		return CIQUEUE_ERR_UNDERFLOW;
	}

	/* Queue is not full, copy element. */
	if( element != NULL ) {
		memcpy(element, &self->_.queueBase[self->_.tail], self->_.element_size);
	}

	/* Increment tail to next element. */
	CCArrayQueue_IncrementTail(self);

	/* Decrease size of queue. */
	--self->_.size;

	return CIQUEUE_OK;
}

static CIQueueError CIQueue_Peek_Def( struct CIQueue* self_, void* element )
{
	struct CCArrayQueue* self = CCast(self_);

	/* Check if the queue is empty. */
	if( self->_.size == 0 ) {
		return CIQUEUE_ERR_UNDERFLOW;
	}

	/* Queue is not full, peek at element. */
	memcpy(element, &self->_.queueBase[self->_.tail], self->_.element_size);

	return CIQUEUE_OK;
}

static size_t CIQueue_Size_Def( struct CIQueue* self_ )
{
	struct CCArrayQueue* self = CCast(self_);

	return self->_.size;
}

static size_t CIQueue_MaxSize_Def( struct CIQueue* self_ )
{
	struct CCArrayQueue* self = CCast(self_);

	return self->_.max_size;
}

static void CIQueue_Clear_Def( struct CIQueue* self_ )
{
	struct CCArrayQueue* self = CCast(self_);

	self->_.head = 0;
	self->_.tail = 0;
	self->_.size = 0;
}


/************************************************************************/
/* Overriding 								*/
/************************************************************************/
static void CDestructor( void* self_ )
{
	struct CCArrayQueue* self = CCast(self_);

	if( !self->_.is_static ) {
		CFree(self->_.queueBase);
	}

	/* Call super's destructor. */
	((struct CCArrayQueue_VTable*) CGetVTable(self))->cobject->CDestructor(self);
}
	

/************************************************************************/
/* vtable key								*/
/************************************************************************/
const struct CCArrayQueue_VTable* CCArrayQueue_GetVTable( )
{
	static struct CCArrayQueue_VTable vtable  =
		{
			/* Assign implemenation of interface CIQueue's methods. */
			.ciqueue_override.insert = CIQueue_Insert_Def,
			.ciqueue_override.remove = CIQueue_Remove_Def,
			.ciqueue_override.peek = CIQueue_Peek_Def,
			.ciqueue_override.size = CIQueue_Size_Def,
			.ciqueue_override.maxSize = CIQueue_MaxSize_Def,
			.ciqueue_override.clear = CIQueue_Clear_Def
		};

	/* Super's vtable copy. */
	vtable.cobject_override = *CObject_GetVTable( );

	/* Override destructor. */
	vtable.cobject_override.CDestructor = CDestructor;

	/* Reference to super's vtable. */
	vtable.cobject = CObject_GetVTable( );

	/* Return pointer to CCArrayQueue's vtable. */
	return &vtable;
}

/************************************************************************/
/* Constructor								*/
/************************************************************************/
CError CCArrayQueue( struct CCArrayQueue* self, size_t element_size, size_t queue_size )
{
	/* First thing in constructor must be to call super's constructor. */
	CObject(&self->cobject);

	/* Second thing in constructor must be to map vtable. */
	CVTable(self, CCArrayQueue_GetVTable( ));

	/* Third thing in constructor must be calling interface's constructor. */
	CInterface(self, &self->ciqueue, &CCArrayQueue_GetVTable( )->ciqueue_override);

	/* Allocate space for the array. */
	self->_.queueBase = CMalloc(queue_size * element_size);
	if( self->_.queueBase == NULL ) {
		return COBJ_ALLOC_FAIL;
	}

	/* Initialize member variables. */
	self->_.element_size = element_size;
	self->_.head = 0;
	self->_.tail = 0;
	self->_.max_size = queue_size;
	self->_.size = 0;

	/* Using malloc, so set this to false. */
	self->_.is_static = 0;

	return COBJ_OK;
}

CError CCArrayQueueStatic
(
	struct CCArrayQueue* self,
	void* queue_memory,
	size_t element_size,
	size_t queue_size
)
{
	/* First thing in constructor must be to call super's constructor. */
	CObject(&self->cobject);

	/* Second thing in constructor must be to map vtable. */
	CVTable(self, CCArrayQueue_GetVTable( ));

	/* Third thing in constructor must be calling interface's constructor. */
	CInterface(self, &self->ciqueue, &CCArrayQueue_GetVTable( )->ciqueue_override);

	/* Space for the array. */
	self->_.queueBase = queue_memory;

	/* Initialize member variables. */
	self->_.element_size = element_size;
	self->_.head = 0;
	self->_.tail = 0;
	self->_.max_size = queue_size;
	self->_.size = 0;

	/* Not using malloc, so set this to true. */
	self->_.is_static = 1;

	return COBJ_OK;	
}
