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

#ifndef UTIL_CCSOFTSERIALBUS_H_
#define UTIL_CCSOFTSERIALBUS_H_

#include <Class.h>
#include <CCSoftSerialDev.h>
#include <CCThreadedQueue.h>
#include <CCBinaryTree.h>


/************************************************************************/
/* Declare Class and vtable.						*/
/************************************************************************/
/**
 * @struct CCSoftSerialBus
 * @ingroup InterTaskCommunication
 * @sa struct CCSoftSerialDev
 * @brief
 *	Implements a bus used to faciliate the communication between
 *	CCSoftSerial objects.
 * @details
 *	Implements a bus used to faciliate the communication between
 *	CCSoftSerial objects. This class' functions should never be
 *	called by the application program.
 */
struct CCSoftSerialBus
{
	/* Super class must always be first member */
	/* of a class' struct. */
	struct CObject cobject;

	/* Private member variables. */
	struct
	{
		CCSoftSerialDevID masterID;
		CCSoftSerialDevID slaveID;
		struct CCThreadedQueue* miso_channel;
		struct CCThreadedQueue* mosi_channel;
		struct CCBinaryTree pending_masters;
		#ifdef __unix__
		pthread_mutex_t device_lock;
		pthread_cond_t select_cond;
		pthread_cond_t idle_cond;
		#else
		#error "Unsupported operating system"
		#endif
	} priv;
};

/**
 * @ingroup VTable
 * @brief
 *	CCSoftSerialBus vtable
 */
struct CCSoftSerialBus_VTable
{
	/* Space for a carbon copy of the super class' virtual table must  */
	/* be the first member of a class virtual table declaration. */
	/* This is where we will override inherited members. */
	struct CObject_VTable cobject_override;

	/* Since we are overriding the destructor, and need to call the */
	/* originally implementation in the new implementation, we need to */
	/* keep a reference to the super class' vtable. */
	const struct CObject_VTable* cobject;
};

/**
 * @memberof CCSoftSerialBus
 * @ingroup VTable
 * @details
 *	Get reference to the struct CCSoftSerialBus's vtable.
 */
const struct CCSoftSerialBus_VTable* CCSoftSerialBus_GetVTable( );


/************************************************************************/
/* Class Methods							*/
/************************************************************************/
/**
 * @memberof CCSoftSerialBus
 * @constructor
 * @sa CCSoftSerialDevMaster()
 * @details
 * @param self
 *	The serial bus to construct.
 * @param tokenSize
 *	The length of data arrays which are written/read by devices with CCSoftSerialDev_Read()
 *	and CCSoftSerialDev_Write().
 * @param channelSize
 *	The number of tokens which can fit into the read/write channel before causing an
 *	overflow condition.
 * @notes
 *	This class requires a binary tree data structure to operate correctly. The current
 *	binary tree interface only implements dynamic construction. Therefore, this will
 *	use CMalloc to create a binary and CFree to release the memory when the bus object
 *	is destroyed.
 */
CError CCSoftSerialBus( struct CCSoftSerialBus* self, size_t tokenSize, size_t channelSize );

/**
 * @memberof CCSoftSerialBus
 * @constructor
 * @note
 *	Master in Slave out (miso) and Master out Slave in (mosi)
 * @details
 *	Same as CCSoftSerialBus, except the programmer is reponsible for constructing the miso and
 *	mosi data channels (the channels are just thread safe queues).
 *	When the destructor for the bus object is called, these queues will be have their destructor called too.
 * @notes
 *	This class requires a binary tree data structure to operate correctly. The current
 *	binary tree interface only implements dynamic construction. Therefore, this will
 *	use CMalloc to create a binary and CFree to release the memory when the bus object
 *	is destroyed.

 */
CError CCSoftSerialBusStatic( struct CCSoftSerialBus* self, struct CCThreadedQueue* misoChannel, struct CCThreadedQueue* mosiChannel );

/**
 * @memberof CCSoftSerialBus
 * @details
 *	Should never be called in application code. This is used to put data on the bus.
 * @param device
 *	The device which is trying to put data on the bus
 */
CCSoftSerialError CCSoftSerialBus_Write( struct CCSoftSerialBus* self, struct CCSoftSerialDev* device, void* data, COS_Timemsec blockTime );

/**
 * @memberof CCSoftSerialBus
 * @details
 *	Should never be called in application code. This is used to read data off the bus.
 * @param device
 *	The device which is trying to read data off the bus.
 */
CCSoftSerialError CCSoftSerialBus_Read( struct CCSoftSerialBus* self, struct CCSoftSerialDev* device, void* data, COS_Timemsec blockTime );

/**
 * @memberof CCSoftSerialBus
 * @details
 *	Should never be called in application code. This is used to select a device
 * @param device
 *	The device which is trying to select a slave.
 * @param id
 *	ID of the device being selected.
 */
CCSoftSerialError CCSoftSerialBus_Select( struct CCSoftSerialBus* self, struct CCSoftSerialDev* device, CCSoftSerialDevID id, COS_Timemsec blockTime );

/**
 * @memberof CCSoftSerialBus
 * @details
 *	Should never be called in application code. This is used to unselect a device.
 * @param device
 *	The device which is unselecting a slave.
 */
CCSoftSerialError CCSoftSerialBus_Unselect( struct CCSoftSerialBus* self, struct CCSoftSerialDev* device );

/**
 * @memberof CCSoftSerialBus
 * @details
 *	Should never be called in application code. This is used to put data on the bus.
 * @param device
 *	The device which is waiting to be selected.
 */
CCSoftSerialError CCSoftSerialBus_Isselected( struct CCSoftSerialBus* self, struct CCSoftSerialDev* device, COS_Timemsec blockTime );


#endif /* UTIL_CCSOFTSERIALBUS_H_ */

