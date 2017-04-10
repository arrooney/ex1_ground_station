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
 * March 27, 2017
 */

#include <CCSoftSerialDev.h>
#include <CCSoftSerialBus.h>

/************************************************************************/
/* Class methods.							*/
/************************************************************************/
CCSoftSerialDevID CCSoftSerialDev_GetID( struct CCSoftSerialDev* self )
{
	CAssertObject(self);
	return self->priv.id;
}

unsigned char CCSoftSerialDev_GetPriority( struct CCSoftSerialDev* self )
{
	CAssertObject(self);
	return self->priv.priority;
}
       

CCSoftSerialError CCSoftSerialDev_Select( struct CCSoftSerialDev* self,
					     CCSoftSerialDevID id,
					     COS_Timemsec blockTime )
{
	struct CCSoftSerialBus* bus;
	CAssertObject(self);

	bus = self->priv.bus;
	return CCSoftSerialBus_Select(bus, self, id, blockTime);
}


CCSoftSerialError CCSoftSerialDev_Unselect( struct CCSoftSerialDev* self )
{
	struct CCSoftSerialBus* bus;
	CAssertObject(self);

	bus = self->priv.bus;
	return CCSoftSerialBus_Unselect(bus, self);
}

CBool CCSoftSerialDev_Isselected( struct CCSoftSerialDev* self, COS_Timemsec blockTime )
{
	struct CCSoftSerialBus* bus;
	CAssertObject(self);

	bus = self->priv.bus;
	return CCSoftSerialBus_Isselected(bus, self, blockTime);
}

CCSoftSerialError CCSoftSerialDev_Write( struct CCSoftSerialDev* self,
					    void* data,
					    COS_Timemsec blockTime )
{
	struct CCSoftSerialBus* bus;
	CAssertObject(self);

	bus = self->priv.bus;
	return CCSoftSerialBus_Write(bus, self, data, blockTime);
}

CCSoftSerialError CCSoftSerialDev_Read( struct CCSoftSerialDev* self,
					   void* data,
					   COS_Timemsec blockTime )
{
	struct CCSoftSerialBus* bus;
	CAssertObject(self);

	bus = self->priv.bus;
	return CCSoftSerialBus_Read(bus, self, data, blockTime);
}


/************************************************************************/
/* vtable key								*/
/************************************************************************/
const struct CCSoftSerialDev_VTable* CCSoftSerialDev_GetVTable( )
{
	static struct CCSoftSerialDev_VTable vtable;

	/* Copy of super's vtable. */
	vtable.cobject_override = *CObject_GetVTable( );

	/* Return pointer to vtable. */
	return &vtable;
}


/************************************************************************/
/* Constructors methods.						*/
/************************************************************************/
CError CCSoftSerialDevMaster( struct CCSoftSerialDev* self,
			      unsigned char priority,
			      CCSoftSerialDevID id,
			      struct CCSoftSerialBus* bus )
{
	/* Construct super class.
	 */
	CObject(&self->cobject);

	/* Bind the vtable to this object. 
	 */
	CVTable(self, CCSoftSerialDev_GetVTable( ));

	/* Bounds check the priority
	 */
	if( priority == CCSOFTSERIALDEV_SLAVE ) {
		priority = CCSOFTSERIALDEV_MIN_PRIO;
	}

	/* NULL pointer check the bus.
	 */
	if( bus == NULL ) {
		return COBJ_INV_PARAM;
	}

	/* Assign member variables.
	 */
	self->priv.id = id;
	self->priv.priority = priority;
	self->priv.bus = bus;
	
	return COBJ_OK;
}

CError CCSoftSerialDevSlave( struct CCSoftSerialDev* self,
			     CCSoftSerialDevID id,
			     struct CCSoftSerialBus* bus )
{
	/* Construct super class.
	 */
	CObject(&self->cobject);

	/* Bind the vtable to this object. 
	 */
	CVTable(self, CCSoftSerialDev_GetVTable( ));

	/* NULL pointer check the bus.
	 */
	if( bus == NULL ) {
		return COBJ_INV_PARAM;
	}

	/* Assign member variables.
	 */
	self->priv.id = id;
	self->priv.priority = CCSOFTSERIALDEV_SLAVE;
	self->priv.bus = bus;
	
	return COBJ_OK;
}
