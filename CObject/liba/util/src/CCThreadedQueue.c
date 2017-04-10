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

#include <CCThreadedQueue.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


/************************************************************************/
/* Private methods							*/
/************************************************************************/
static CError CCThreadedQueue_CreateOSObjects( struct CCThreadedQueue* self )
{
	CAssertObject(self);

#ifdef __unix__
	/* OS initialization for unix systems */
	/* default mutex */
	if( pthread_mutex_init(&self->_.mutex, NULL) != 0 ) {
		return COBJ_EXT_FAIL;
	}

	/* Conditional variable. */
	if( pthread_cond_init(&self->_.insertCondition, NULL) != 0 ) {
		pthread_mutex_destroy(&self->_.mutex);
		return COBJ_EXT_FAIL;
	}
	if( pthread_cond_init(&self->_.removeCondition, NULL) != 0 ) {
		pthread_cond_destroy(&self->_.insertCondition);
		pthread_mutex_destroy(&self->_.mutex);
		return COBJ_EXT_FAIL;
	}
#endif

	return COBJ_OK;
}

static void CCThreadedQueue_DestroyOSObjects( struct CCThreadedQueue* self )
{
	CAssertObject(self);

#ifdef __unix__
	pthread_mutex_destroy(&self->_.mutex);
	pthread_cond_destroy(&self->_.insertCondition);
	pthread_cond_destroy(&self->_.removeCondition);
#endif
}

static void CCThreadedQueue_ResetOSObjects( struct CCThreadedQueue* self )
{
	CAssertObject(self);
	pthread_cond_broadcast(&self->_.insertCondition);
	pthread_cond_broadcast(&self->_.removeCondition);
}

static void CCThreadedQueue_QueryLock( struct CCThreadedQueue* self )
{
	CAssertObject(self);
	pthread_mutex_lock(&self->_.mutex);
}

static void CCThreadedQueue_QueryUnlock( struct CCThreadedQueue* self )
{
	CAssertObject(self);
	pthread_mutex_unlock(&self->_.mutex);
}

static CCTQueueError CCThreadedQueue_RemoveLock( struct CCThreadedQueue* self, COS_Timemsec blockTime )
{
	struct timespec abstime;
	
	CAssertObject(self);

	if( blockTime == COS_BLOCK_FOREVER ) {
		pthread_mutex_lock(&self->_.mutex);
	}
	else {
		clock_gettime(CLOCK_REALTIME, &abstime);
		abstime.tv_sec += blockTime / MS_PER_SECOND;
		abstime.tv_nsec += (blockTime % MS_PER_SECOND) * NS_PER_MS;

		if( pthread_mutex_timedlock(&self->_.mutex, &abstime) != 0 ) {
			/* Timed out. */
			return CCTQUEUE_ERR_TIMEOUT;
		}
	}

	for( ;; ) {
		/* We have the mutex, Check to see if there are elements to remove. */
		/* If not, wait on the conditional variable. */
		if( CIQueue_Size(self->_.backbone) > 0 ) {
			pthread_mutex_unlock(&self->_.mutex);
			return CCTQUEUE_OK;
		}

		/* No elements to remove, block on conditional variable. */
		if( blockTime == COS_BLOCK_FOREVER ) {
			pthread_cond_wait(&self->_.removeCondition, &self->_.mutex);
		}
		else {
			if( pthread_cond_timedwait(&self->_.removeCondition, &self->_.mutex, &abstime) != 0 ) {
				/* Timed out. */
				pthread_mutex_unlock(&self->_.mutex);
				return CCTQUEUE_ERR_TIMEOUT;
			}
		}
	}

	/* We have the mutex and have been signaled there is data in the queue to pop. */
	return CCTQUEUE_OK;
}

static void CCThreadedQueue_RemoveUnlock( struct CCThreadedQueue* self )
{
	CAssertObject(self);

	pthread_mutex_unlock(&self->_.mutex);

	/* Data was popped off the queue. Send the signal that there is room in the queue. */
	pthread_cond_broadcast(&self->_.insertCondition);
}

static CCTQueueError CCThreadedQueue_InsertLock( struct CCThreadedQueue* self, COS_Timemsec blockTime )
{
	struct timespec abstime;
	
	CAssertObject(self);

	if( blockTime == COS_BLOCK_FOREVER ) {
		pthread_mutex_lock(&self->_.mutex);
	}
	else {
		clock_gettime(CLOCK_REALTIME, &abstime);
		abstime.tv_sec += blockTime / MS_PER_SECOND;
		abstime.tv_nsec += (blockTime % MS_PER_SECOND) * NS_PER_MS;

		if( pthread_mutex_timedlock(&self->_.mutex, &abstime) != 0 ) {
			/* Timed out. */
			return CCTQUEUE_ERR_TIMEOUT;
		}
	}

	for( ;; ) {
		/* We have the mutex, Check to see if there room to add elements. */
		if( CIQueue_Size(self->_.backbone) < CIQueue_MaxSize(self->_.backbone) ) {
			pthread_mutex_unlock(&self->_.mutex);
			return CCTQUEUE_OK;
		}

		/* No room to add elements, block on conditional variable. */
		if( blockTime == COS_BLOCK_FOREVER ) {
			pthread_cond_wait(&self->_.insertCondition, &self->_.mutex);
		}
		else {
			if( pthread_cond_timedwait(&self->_.insertCondition, &self->_.mutex, &abstime) != 0 ) {
				/* Timed out. */
				pthread_mutex_unlock(&self->_.mutex);
				return CCTQUEUE_ERR_TIMEOUT;
			}
		}
	}

	/* We have the mutex and have been signaled there is room to add data. */
	return CCTQUEUE_OK;
}

static void CCThreadedQueue_InsertUnlock( struct CCThreadedQueue* self )
{
	CAssertObject(self);

	pthread_mutex_unlock(&self->_.mutex);

	/* Data was inserted into the queue. Send the signal that there is data to remove. */
	pthread_cond_broadcast(&self->_.removeCondition);
}

/************************************************************************/
/* Methods								*/
/*************************************************************************/
CCTQueueError CCThreadedQueue_Insert( struct CCThreadedQueue* self, const void* element, COS_Timemsec blockTime )
{
	CAssertObject(self);

	if( CCThreadedQueue_InsertLock(self, blockTime) == CCTQUEUE_ERR_TIMEOUT ) {
		return CCTQUEUE_ERR_TIMEOUT;
	}

	CIQueueError err = CIQueue_Insert(self->_.backbone, element);
	CCThreadedQueue_InsertUnlock(self);

	return (CCTQueueError) err;
}

CCTQueueError CCThreadedQueue_Remove( struct CCThreadedQueue* self, void* element, COS_Timemsec blockTime )
{
	CAssertObject(self);

	if( CCThreadedQueue_RemoveLock(self, blockTime) == CCTQUEUE_ERR_TIMEOUT ) {
		return CCTQUEUE_ERR_TIMEOUT;
	}

	CIQueueError err = CIQueue_Remove(self->_.backbone, element);
	CCThreadedQueue_RemoveUnlock(self);

	return (CCTQueueError) err;
}

CCTQueueError CCThreadedQueue_Peek( struct CCThreadedQueue* self, void* element, COS_Timemsec blockTime )
{
	CAssertObject(self);

	if( CCThreadedQueue_RemoveLock(self, blockTime) == CCTQUEUE_ERR_TIMEOUT ) {
		return CCTQUEUE_ERR_TIMEOUT;
	}

	CIQueueError err = CIQueue_Peek(self->_.backbone, element);
	CCThreadedQueue_RemoveUnlock(self);

	return (CCTQueueError) err;
}

size_t CCThreadedQueue_Size( struct CCThreadedQueue* self )
{
	CAssertObject(self);

	CCThreadedQueue_QueryLock(self);
	size_t size = CIQueue_Size(self->_.backbone);
	CCThreadedQueue_QueryUnlock(self);

	return size;
}

size_t CCThreadedQueue_MaxSize( struct CCThreadedQueue* self )
{
	CAssertObject(self);

	CCThreadedQueue_QueryLock(self);
	size_t size = CIQueue_MaxSize(self->_.backbone);
	CCThreadedQueue_QueryUnlock(self);

	return size;
}

void CCThreadedQueue_Clear( struct CCThreadedQueue* self )
{
	CAssertObject(self);

	CCThreadedQueue_QueryLock(self);
	CIQueue_Clear(self->_.backbone);

	/* Reset semaphores. */
	CCThreadedQueue_ResetOSObjects(self);

	CCThreadedQueue_QueryUnlock(self);
}


/************************************************************************/
/* Overriding 								*/
/************************************************************************/
static void CDestructor( void* self_ )
{
	struct CCThreadedQueue* self = CCast(self_);

	CCThreadedQueue_DestroyOSObjects(self);
	CDestroy(self->_.backbone);

	/* Call super's destructor. */
	((struct CCThreadedQueue_VTable*) CGetVTable(self))->cobject->CDestructor(self);
}
	

/************************************************************************/
/* vtable key								*/
/************************************************************************/
const struct CCThreadedQueue_VTable* CCThreadedQueue_GetVTable( )
{
	static struct CCThreadedQueue_VTable vtable;

	/* Super's vtable copy. */
	vtable.cobject_override = *CObject_GetVTable( );

	/* Override destructor of super class. */
	vtable.cobject_override.CDestructor = CDestructor;

	/* Reference to super's vtable. */
	vtable.cobject = CObject_GetVTable( );

	/* Return pointer to CCArrayQueue's vtable. */
	return &vtable;
}

/************************************************************************/
/* Constructor								*/
/************************************************************************/
CError CCThreadedQueue( struct CCThreadedQueue* self, struct CIQueue* backbone )
{
	/* First thing in constructor must be to call super's constructor. */
	CObject(&self->cobject);

	/* Second thing in constructor must be to map vtable. */
	CVTable(self, CCThreadedQueue_GetVTable( ));

	/* Set backbone. */
	self->_.backbone = backbone;

	return CCThreadedQueue_CreateOSObjects(self);
}
