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
 * March 24, 2017
 */
/**
 * @file
 */
#include <CCSoftSerialBus.h>
#include <CCArrayQueue.h>

/************************************************************************/
/* Private Class Methods						*/
/************************************************************************/
static CError CCSoftSerialBus_CreateLocks( struct CCSoftSerialBus* self )
{
	CAssertObject(self);

	if( pthread_mutex_init(&self->priv.device_lock, NULL) != 0 ) {
		return COBJ_EXT_FAIL;
	}
	if( pthread_cond_init(&self->priv.select_cond, NULL) != 0 ) {
		pthread_mutex_destroy(&self->priv.device_lock);
		return COBJ_EXT_FAIL;
	}
	if( pthread_cond_init(&self->priv.idle_cond, NULL) != 0 ) {
		pthread_mutex_destroy(&self->priv.device_lock);
		pthread_cond_destroy(&self->priv.select_cond);
		return COBJ_EXT_FAIL;
	}
	if( pthread_cond_init(&self->priv.miso_read_cond, NULL) != 0 ) {
		pthread_mutex_destroy(&self->priv.device_lock);
		pthread_cond_destroy(&self->priv.select_cond);
		pthread_cond_destroy(&self->priv.idle_cond);
		return COBJ_EXT_FAIL;
	}
	if( pthread_cond_init(&self->priv.mosi_write_cond, NULL) != 0 ) {
		pthread_mutex_destroy(&self->priv.device_lock);
		pthread_cond_destroy(&self->priv.select_cond);
		pthread_cond_destroy(&self->priv.idle_cond);
		pthread_cond_destroy(&self->priv.miso_read_cond);
		return COBJ_EXT_FAIL;
	}       
	return COBJ_OK;
}

/* The tree heapifies the smaller keys to the root, therefore
 * we will have to invert the evaluation so that larger keys
 * get heapified to the root.
 */
static signed char CCSoftSerialBus_PriorityCompare( const void* key1_p, const void* key2_p )
{
	unsigned char key1 = *((unsigned char*) key1_p);
	unsigned char key2 = *((unsigned char*) key2_p);

	if( key1 > key2 ) {
		return -1;
	}
	else if( key1 < key2 ) {
		return 1;
	}
	else {
		return 0;
	}
}

static CError CCSoftSerialBus_CommonConst( struct CCSoftSerialBus* self,
			       struct CCThreadedQueue* miso_channel,
			       struct CCThreadedQueue* mosi_channel )
{
	CError err;
	CAssertObject(self);
	
	/* Assert non NULL channel inputs. 
	 */
	if( miso_channel == NULL || mosi_channel == NULL ) {
		return COBJ_INV_PARAM;
	}
	self->priv.miso_channel = miso_channel;
	self->priv.mosi_channel = mosi_channel;

	/* Create binary tree for pending masters.
	 */
	err = CCBinaryTree(&self->priv.pending_masters,
			      sizeof(CCSoftSerialDevID),
			      CCSOFTSERIAL_MAX_PENDING_MASTERS,
			      CCSoftSerialBus_PriorityCompare,
			      sizeof(unsigned char));
	if( err != COBJ_OK ) {
		return err;
	}

	err = CCSoftSerialBus_CreateLocks(self);
	if( err != COBJ_OK ) {
		CDestroy(&self->priv.pending_masters);
		return err;
	}
	
	/* Initially, the bus is unused.
	 */
	self->priv.masterID = CCSOFTSERIAL_NO_ID;
	self->priv.slaveID = CCSOFTSERIAL_NO_ID;

	return COBJ_OK;
}

static CError CCSoftSerialBus_CreateChannel( struct CCSoftSerialBus* self,
					     struct CCThreadedQueue **temp_channel,
					     size_t token_size,
					     size_t channel_size )
{		
	struct CCArrayQueue* channel_backend;
	struct CCThreadedQueue* channel;
	CError err;

	CAssertObject(self);
	if( temp_channel == NULL ) {
		return COBJ_INV_PARAM;
	}
	
	/* Create the backend.
	 */
	channel_backend = CMalloc(sizeof(*channel_backend));
	if( channel_backend == NULL ) {
		return COBJ_ALLOC_FAIL;
	}
	err = CCArrayQueue(channel_backend, token_size, channel_size);
	if( err != COBJ_OK ) {
		CFree(channel_backend);
		return err;
	}
	CDynamic(channel_backend);

	/* Create the thread safe channel
	 */
	channel = CMalloc(sizeof(*channel));
	if( channel == NULL ) {
		CDestroy(channel_backend);
		return COBJ_ALLOC_FAIL;
	}
	err = CCThreadedQueue(channel, &channel_backend->ciqueue);
	if( err != COBJ_OK ) {
		CDestroy(channel_backend);
		CFree(channel);
		return err;
	}
	CDynamic(channel);

	*temp_channel = channel;
	return COBJ_OK;
}


/************************************************************************/
/* Class Methods							*/
/************************************************************************/
/* Cyclic complexity map:
 * 1(bounds check) -> 2(if master) ----------------+--> 3(write) -> 4(return) <--+
 *                                                 |                             |
 * +-- 6(if insert ok) <--+--<--+-- 5(if slave) <--+                             |
 * |                      |     |                                                |
 * |                      |     +---------------------> 10(not slave) -------->--+
 * |                      |                                                      |
 * |                      +--<-------------------------+                         |
 * |                                                   |                         |
 * +--> 7(block) -> 8(if timeout) --+--> 9(if slave) --+---------------------->--+
 *                                  |                                            |
 *                                  +----------------------------------------->--+
 *                                
 * Complexity of: 13(edges) - 10(nodes) + 2 = 5
 * Test cases:
 *	* Slave writing
 *	* Master writing
 *	* Unauthorized device writing
 *	* Slave reselect while slave blocking to write
 *	* Slave timeout blocking to write
 *	* Master timeout blocking to write
 */
CCSoftSerialError CCSoftSerialBus_Write( struct CCSoftSerialBus* self,
					 struct CCSoftSerialDev* querying_device,
					 void* data,
					 COS_Timemsec block_time )
{
	CCSoftSerialDevID deviceID;
	CCTQueueError err;
	struct CCThreadedQueue* channel;
	struct timespec abstime;
	CAssertObject(self);

	if( querying_device == NULL ) {
		return CCSOFTSERIAL_ERR_INV_PARAM;
	}
	
	/* Get mutual exclusion on the bus.
	 */
	pthread_mutex_lock(&self->priv.device_lock);
	

	/* At this point, the querying device is a valid device. Write
	 * to the channel.
	 */
	deviceID = CCSoftSerialDev_GetID(querying_device);
	if( deviceID == self->priv.masterID ) {
		/* querying device is the bus master.
		 */
		channel = self->priv.mosi_channel;
		pthread_mutex_unlock(&self->priv.device_lock);
		err = CCThreadedQueue_Insert(channel, data, block_time);
		if( err == CCTQUEUE_ERR_TIMEOUT ) {
			return CCSOFTSERIAL_ERR_TIMEOUT;
		}
		pthread_cond_broadcast(&self->priv.mosi_write_cond);
		return CCSOFTSERIAL_OK;
	}
	else if( deviceID == self->priv.slaveID ) {
		/* querying device is the slave.
		 */
		channel = self->priv.miso_channel;

		/* Try to add message to the channel. If it's full
		 * we have to use a conditional variable to block
		 * on. If the slave device blocked on the queue instead
		 * and the master releases the bus, the slave will write a message
		 * in here (because unselect clears these channels). The next master
		 * to read out the bus will read that message. We don't want that.
		 * We want the block to be interrupted if the master releases the bus.
		 */
		clock_gettime(CLOCK_REALTIME, &abstime);
		abstime.tv_sec += block_time / MS_PER_SECOND;
		abstime.tv_nsec += (block_time % MS_PER_SECOND) * NS_PER_MS;
		for( ;; ) {
			err = CCThreadedQueue_Insert(channel, data, COS_NO_BLOCK);
			if( err == CCTQUEUE_OK ) {
				pthread_mutex_unlock(&self->priv.device_lock);
				return CCSOFTSERIAL_OK;
			}

			if( block_time == COS_BLOCK_FOREVER ) {
				pthread_cond_wait(&self->priv.miso_read_cond, &self->priv.device_lock);
			}
			else {
				err = pthread_cond_timedwait(&self->priv.miso_read_cond, &self->priv.device_lock, &abstime);
				if( err != 0 ) {
					/* Timed out. 
					 */
					pthread_mutex_unlock(&self->priv.device_lock);
					return CCSOFTSERIAL_ERR_TIMEOUT;
				}
			}

			if( deviceID != self->priv.slaveID ) {
				/* Master released bus before this write could happen. 
				 */
				pthread_mutex_unlock(&self->priv.device_lock);
				return CCSOFTSERIAL_ERR_TIMEOUT;
			}

		}
	}
	else {
		/* Not the current slave or master.
		 */
		pthread_mutex_unlock(&self->priv.device_lock);
		return CCSOFTSERIAL_ERR_PRIV;
	}
	return CCSOFTSERIAL_OK;
}

/* Same algorithm as CCSoftSerialBus_Write. Use the same test
 * cases but for reading instead:
 *	* Slave device reading
 *	* Master device reading
 *	* Unauthorized device reading
 *	* Slave reselect while slave blocking to read
 *	* Slave timeout blocking to read
 *	* Master timeout blocking to read
 */
CCSoftSerialError CCSoftSerialBus_Read( struct CCSoftSerialBus* self,
					struct CCSoftSerialDev* querying_device,
					void* data,
					COS_Timemsec block_time )
{
	CCSoftSerialDevID deviceID;
	CCTQueueError err;
	struct CCThreadedQueue* channel;
	struct timespec abstime;
	CAssertObject(self);

	if( querying_device == NULL ) {
		return CCSOFTSERIAL_ERR_INV_PARAM;
	}
	
	/* Get mutual exclusion on the bus.
	 */
	pthread_mutex_lock(&self->priv.device_lock);
	
	/* At this point, the querying device is a valid device. Read
	 * from the channel.
	 */
	deviceID = CCSoftSerialDev_GetID(querying_device);
	if( deviceID == self->priv.slaveID ) {
		/* querying device is the slave.
		 */
		channel = self->priv.mosi_channel;

		/* Try to from the channel. If it's empty
		 * we have to use a conditional variable to block
		 * on. If the slave device blocked on the queue instead
		 * and the master releases the bus, the slave will wrongly read the next 
		 * message written to the bus. The next message will be meant for a different slave
		 * though.
		 */
		clock_gettime(CLOCK_REALTIME, &abstime);
		abstime.tv_sec += block_time / MS_PER_SECOND;
		abstime.tv_nsec += (block_time % MS_PER_SECOND) * NS_PER_MS;
		for( ;; ) {
			err = CCThreadedQueue_Remove(channel, data, COS_NO_BLOCK);
			if( err == CCTQUEUE_OK ) {
				pthread_mutex_unlock(&self->priv.device_lock);
				return CCSOFTSERIAL_OK;
			}

			if( block_time == COS_BLOCK_FOREVER ) {
				pthread_cond_wait(&self->priv.mosi_write_cond, &self->priv.device_lock);
			}
			else {
				err = pthread_cond_timedwait(&self->priv.mosi_write_cond, &self->priv.device_lock, &abstime);
				if( err != 0 ) {
					/* Timed out. 
					 */
					pthread_mutex_unlock(&self->priv.device_lock);
					return CCSOFTSERIAL_ERR_TIMEOUT;
				}
			}

			if( deviceID != self->priv.slaveID ) {
				/* Master released bus before this read could happen. 
				 */
				pthread_mutex_unlock(&self->priv.device_lock);
				return CCSOFTSERIAL_ERR_TIMEOUT;
			}

		}		
	}
	else if( deviceID == self->priv.masterID ) {
		/* querying device is the bus master.
		 */
		channel = self->priv.miso_channel;
		pthread_mutex_unlock(&self->priv.device_lock);
		err = CCThreadedQueue_Remove(channel, data, block_time);
		if( err == CCTQUEUE_ERR_TIMEOUT ) {
			return CCSOFTSERIAL_ERR_TIMEOUT;
		}
		pthread_cond_broadcast(&self->priv.miso_read_cond);
		return CCSOFTSERIAL_OK;
	}
	else {
		/* Not the current slave or master.
		 */
		pthread_mutex_unlock(&self->priv.device_lock);
		return CCSOFTSERIAL_ERR_PRIV;
	}
	return CCSOFTSERIAL_OK;      
}

/* Cyclic complexit map:
 * 1(bounds check) -> 2(if slave) -----------+
 *                                           |                  
 *    +--> 8(take bus) -->-+--> 3(return) <--+--> 10(if reselect) --+
 *    |                    |                                        |
 *    |                    +-<--------------------------------------+----------+
 *    |                    |                                                   |
 *    |                    +-<--------------------------+-- 4(if tree full) <--+
 *    |                                                 |
 *    |                                                 |
 *    +-- 7(if highest prio) <- 6(if idle) <- 5(for) <--+
 *    |                                                 |
 *    +--> 9(block until idle) ----------------------->-+
 *
 * Complexity of: 13(edges) - 10(nodes) + 2 = 5
 * Test cases:
 *	* Querying device is a slave
 *	* Pending masters tree is full
 *	* Pending master is not highest priority when bus goes idle.
 *	* Pending master is highest priority when bus goes idle
 *		- Bus is initially idle 
 *		- Bus is initally busy
 *	* Querying device already owns bus, doing a reselect.
 *
 * An additional test not captured by the complexity map:
 *	* If highest priority master times out, it correctly
 *	  removes itself from the pending tree so that next
 *	  highest priority master can take the bus when it
 *	  becomes idle.
 */
CCSoftSerialError CCSoftSerialBus_Select( struct CCSoftSerialBus* self,
					  struct CCSoftSerialDev* querying_device,
					  CCSoftSerialDevID slave_id,
					  COS_Timemsec block_time )
{
	struct timespec abstime;
	CCSoftSerialDevID querying_master_id;
	int err;
	CCSoftSerialDevID highest_priority_master;
	unsigned char querying_master_prio;
	CITreeError tree_err;
	CAssertObject(self);

	if( querying_device == NULL ) {
		return CCSOFTSERIAL_ERR_INV_PARAM;
	}

	querying_master_id = CCSoftSerialDev_GetID(querying_device);
	querying_master_prio = CCSoftSerialDev_GetPriority(querying_device);
	if( querying_master_prio == CCSOFTSERIALDEV_SLAVE ) {
		/* Slave device attempting to make a selection.
		 */
		return CCSOFTSERIAL_ERR_PRIV;
	}

	/* At this point, the querying device is a valid master. Aquire 
	 * mututal exclusion on member variables before checking for
	 * bus activity.
	 */
	pthread_mutex_lock(&self->priv.device_lock);

	/* Does the querying device already own the bus?
	 */
	if( querying_master_id == self->priv.masterID ) {
		/* The device alreayd owns the bus, do a reselect.
		 */
		self->priv.slaveID = slave_id;

		/* Broadcast to blocking slaves that a new device has 
		 * been selected.
		 */
		pthread_cond_broadcast(&self->priv.select_cond);
		pthread_mutex_unlock(&self->priv.device_lock);
		return CCSOFTSERIAL_OK;
	}
	
	/* Have mutual exclusion, put querying device into the tree of pending masters.
	 */
	tree_err = CITree_Push(&self->priv.pending_masters.citree, &querying_master_id, &querying_master_prio);
	if( tree_err == CITREE_ERR_FULL ) {
		pthread_mutex_unlock(&self->priv.device_lock);
		return CCSOFTSERIAL_ERR_OVRLD;
	}
	if( tree_err != CITREE_OK ) {
		pthread_mutex_unlock(&self->priv.device_lock);
		return CCSOFTSERIAL_ERR_EXT;
	}

	/* Block until the bus can be taken or a timeout occurs.
	 */
	for( ;; ) {
		/* Since we currently have the mutex, we can check for bus activity right
		 * now instead of going straight to a block.
		 */
		if( self->priv.masterID == CCSOFTSERIAL_NO_ID ) {
			/* No bus activity.
			 * Check if the querying device is the  highest priority master blocking on the bus
			 */
			CITree_Peek(&self->priv.pending_masters.citree, &highest_priority_master);
			if( querying_master_id == highest_priority_master ) {
				/* The querying device is the highest priority master blocking on
				 * the bus. Remove it from the pending masters as it is going to take
				 * the bus now.
				 */
				CITree_Pop(&self->priv.pending_masters.citree, NULL);

				/* Setup who is master and slave
				 */
				self->priv.masterID = querying_master_id;
				self->priv.slaveID = slave_id;

				/* Broadcast to blocking slaves that a new bus master has been
				 * selected.
				 */
				pthread_cond_broadcast(&self->priv.select_cond);
				pthread_mutex_unlock(&self->priv.device_lock);
				return CCSOFTSERIAL_OK;
			}
		}

		/* The bus is currently being used. Wait until signalled that the bus is idle. 
		 */
		if( block_time == COS_BLOCK_FOREVER ) {
			pthread_cond_wait(&self->priv.idle_cond, &self->priv.device_lock);
		}
		else {
			clock_gettime(CLOCK_REALTIME, &abstime);
			abstime.tv_sec += block_time / MS_PER_SECOND;
			abstime.tv_nsec += (block_time % MS_PER_SECOND) * NS_PER_MS;
			err = pthread_cond_timedwait(&self->priv.idle_cond, &self->priv.device_lock, &abstime);
			if( err != 0 ) {
				/* Timed out. Remove the querying device from the tree of 
				 * pending masters
				 */
				CITree_DeleteElement(&self->priv.pending_masters.citree, &querying_master_id);
				pthread_mutex_unlock(&self->priv.device_lock);
				return CCSOFTSERIAL_ERR_TIMEOUT;
			}
		}
	}
}

/* Very simple complexity map, not going to write it out.
 * Test cases:
 *	* Querying device is current bus master
 *	* Querying device is not current bus master
 */
CCSoftSerialError CCSoftSerialBus_Unselect( struct CCSoftSerialBus* self,
					    struct CCSoftSerialDev* querying_device )
{
	CCSoftSerialError err;
	CAssertObject(self);

	/* Parameter bounds check.
	 */
	if( querying_device == NULL ) {
		return CCSOFTSERIAL_ERR_INV_PARAM;
	}

	/* Get mutual exclusion.
	 */
	pthread_mutex_lock(&self->priv.device_lock);

	/* Check the querying device is the current bus master and therefore has
	 * the permissions to unselect the bus.
	 */
	if( CCSoftSerialDev_GetID(querying_device) == self->priv.masterID ) {
		/* The querying device is indeed the current bus master.
		 * release the bus and signal the idle bus condition. Clear
		 * the queues.
		 */
		self->priv.slaveID = CCSOFTSERIAL_NO_ID;
		self->priv.masterID = CCSOFTSERIAL_NO_ID;
		CCThreadedQueue_Clear(self->priv.miso_channel);
		CCThreadedQueue_Clear(self->priv.mosi_channel);
		pthread_cond_broadcast(&self->priv.idle_cond);
		pthread_cond_broadcast(&self->priv.miso_read_cond);
		pthread_cond_broadcast(&self->priv.mosi_write_cond);
		err = CCSOFTSERIAL_OK;
	}
	else {
		err = CCSOFTSERIAL_ERR_PRIV;
	}
	pthread_mutex_unlock(&self->priv.device_lock);

	return err;
}

/* Cyclic complexity map:
 * 1(bounds check) -> 7(master check) -+---------------------------------+
 *                                     |                                 |
 *                   +-----------------+                                 |
 *                   |                                                   |
 *                   +--> 2(for) -> 3(check) ----------+--> 4(return) <--+
 *                   ^                                 |                 |
 *                   +-- 6(timeout) <- 5(cond wait) <--+                 |
 *                   v                                                   |
 *                   +---------------------------------------------------+
 * complexity of: 9(edges) - 7(nodes) + 2 = 4
 * test cases:
 *	* Is current master
 *	* Is current selected slave
 *	* Is not current slave, block until becoming slave
 *	* Is not current slave, block and timeout waiting to become slave
 */
CCSoftSerialError CCSoftSerialBus_Isselected( struct CCSoftSerialBus* self,
					      struct CCSoftSerialDev* querying_device,
					      COS_Timemsec block_time )
{
	struct timespec abstime;
	CCSoftSerialDevID dev_id;
	int err;
	
	/* Parameter bounds checks.
	 */
	CAssertObject(self);
	if( querying_device == NULL ) {
		return CCSOFTSERIAL_ERR_INV_PARAM;
	}


	/* Get mutual exclusion on member variables.
	 */
	pthread_mutex_lock(&self->priv.device_lock);

	/* If we are the master device, no need to block until bus master 
	 * changes.
	 */
	dev_id = CCSoftSerialDev_GetID(querying_device);
	if( dev_id == self->priv.masterID ) {
		pthread_mutex_unlock(&self->priv.device_lock);
		return CCSOFTSERIAL_OK;
	}

	/* Block until selected or timeout occurs.
	 */
	for( ;; ) {
		/* Since we currently have the mutex, check if we're selected before blocking.
		 * If the current slave is the same as the querying device, there is
		 * nothing to do but return sucessfully.
		 */
		if( CCSoftSerialDev_GetID(querying_device) == self->priv.slaveID ) {
			pthread_mutex_unlock(&self->priv.device_lock);
			return CCSOFTSERIAL_OK;
		}

		/* The querying device is not the current slave. Wait until we're signalled
		 * that a new slave has been selected.
		 */
		if( block_time == COS_BLOCK_FOREVER ) {
			pthread_cond_wait(&self->priv.select_cond, &self->priv.device_lock);
		}
		else {
			clock_gettime(CLOCK_REALTIME, &abstime);
			abstime.tv_sec += block_time / MS_PER_SECOND;
			abstime.tv_nsec += (block_time % MS_PER_SECOND) * NS_PER_MS;
			err = pthread_cond_timedwait(&self->priv.select_cond, &self->priv.device_lock, &abstime);
			if( err != 0 ) {
				/* Timed out.
				 */
				pthread_mutex_unlock(&self->priv.device_lock);
				return CCSOFTSERIAL_ERR_TIMEOUT;
			}
		}
	}
}


/************************************************************************/
/* Overriding 								*/
/************************************************************************/
static void CDestructor( void* self_ )
{
	struct CCSoftSerialBus* self = CCast(self_);

	CDestroy(self->priv.miso_channel);
	CDestroy(self->priv.mosi_channel);
	CDestroy(&self->priv.pending_masters);
	pthread_mutex_destroy(&self->priv.device_lock);
	pthread_cond_destroy(&self->priv.select_cond);
	pthread_cond_destroy(&self->priv.idle_cond);

	/* Call super's destructor
	 */
	const struct CCSoftSerialBus_VTable* vtable = CGetVTable(self);
	vtable->cobject->CDestructor(self);
}


/************************************************************************/
/* vtable key								*/
/************************************************************************/
const struct CCSoftSerialBus_VTable* CCSoftSerialBus_GetVTable( )
{
	static struct CCSoftSerialBus_VTable vtable;

	/* Inherit super's methods
	 */
	vtable.cobject_override = *CObject_GetVTable( );

	/* Override Destructor
	 */
	vtable.cobject_override.CDestructor = CDestructor;

	/* Keep reference to super's original vtable.
	 * so that we can cascade the destructor.
	 */
	vtable.cobject = CObject_GetVTable( );
	
	/* Return pointer to vtable. */
	return &vtable;
}


/************************************************************************/
/* Constructors Methods							*/
/************************************************************************/
CError CCSoftSerialBus( struct CCSoftSerialBus* self,
			size_t token_size,
			size_t channel_size )
{
	struct CCThreadedQueue* miso_channel;
	struct CCThreadedQueue* mosi_channel;
	CError err;
	
	CAssertObject(self);

	/* Construct super class.
	 */
	CObject(&self->cobject);

	/* Bind this classes vtable to the object.
	 */
	CVTable(self, CCSoftSerialBus_GetVTable( ));

	/* Create miso channel.
	 */
	err = CCSoftSerialBus_CreateChannel(self, &miso_channel, token_size, channel_size);
	if( err != COBJ_OK ) {
		return err;
	}

	/* Create mosi channel.
	 */
	err = CCSoftSerialBus_CreateChannel(self, &mosi_channel, token_size, channel_size);
	if( err != COBJ_OK ) {
		CDestroy(miso_channel);
		return err;
	}

	err = CCSoftSerialBus_CommonConst(self, miso_channel, mosi_channel);
	if( err != COBJ_OK ) {
		CDestroy(miso_channel);
		CDestroy(mosi_channel);
	}
	return err;
}

CError CCSoftSerialBusStatic( struct CCSoftSerialBus* self,
			      struct CCThreadedQueue* miso_channel,
			      struct CCThreadedQueue* mosi_channel )
{
	CAssertObject(self);

	/* Construct super class.
	 */
	CObject(&self->cobject);

	/* Bind this classes vtable to the object.
	 */
	CVTable(self, CCSoftSerialBus_GetVTable( ));

        return CCSoftSerialBus_CommonConst(self, miso_channel, mosi_channel);
}


