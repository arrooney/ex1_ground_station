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
 * Jun 20, 2016
 */

#include "virtual_test_classes.h"

/****************************************************************************/
/* Class A																	*/
/****************************************************************************/
static int method0( struct VTClassA* self )
{
	CAssertObject(self);
	return VT_CLASSA_METHOD0;
}

static int method1( struct VTClassA* self )
{
	CAssertObject(self);
	return VT_CLASSA_METHOD1;
}

static int method2( struct VTClassA* self )
{
	CAssertObject(self);
	return VT_CLASSA_METHOD2;
}

static int method3( struct VTClassA* self )
{
	CAssertObject(self);
	return VT_CLASSA_METHOD3;
}

static int method4( struct VTClassA* self )
{
	CAssertObject(self);
	return VT_CLASSA_METHOD4;
}

const struct VTClassA_VTable* VTClassA_VTable_Key( )
{
	/* Only need one vtable for every instance of this class. */
	static struct VTClassA_VTable vtable;

	/* Get a copy of the super's vtable for this class. */
	vtable.CObject_VTable = *CObject_GetVTable( );

	/* Link all of this class' virtual methods. */
	vtable.method0 = method0;
	vtable.method1 = method1;
	vtable.method2 = method2;
	vtable.method3 = method3;
	vtable.method4 = method4;

	/* Return pointer. */
	return &vtable;
}

void newVTClassA( struct VTClassA* self )
{
	CAssertObject(self);

	/* Call super's constructor. */
	CObject(&self->cobject);

	/* Map vtable. */
	CVTable(self, VTClassA_VTable_Key( ));
}

/****************************************************************************/
/* Class B																	*/
/****************************************************************************/
static int classBMethod1( struct VTClassA* self )
{
	CAssertObject(self);
	return VT_CLASSB_METHOD1;
}

static int classBMethod2( struct VTClassA* self_ )
{
	/* This is VTClassB's implementation, so cast object back to type VTClassB. */
	struct VTClassB* self = CCast(self_);

	/* Call super's (VTClassA)  implementation of this method. */
	return VT_CLASSB_METHOD2 + ((struct VTClassB_VTable*) CGetVTable(self))->Supers_VTClassA_VTable->method2(&self->classA);
}

static int classBMethod3( struct VTClassA* self )
{
	CAssertObject(self);
	return VT_CLASSB_METHOD3;
}

static int classBMethod4( struct VTClassA* self_ )
{
	/* This is VTClassB's implementation, so cast object back to type VTClassB. */
	struct VTClassB* self = CCast(self_);

	/* Call super's (VTClassA) implementation of this method. */
	return VT_CLASSB_METHOD4 + ((struct VTClassB_VTable*) CGetVTable(self))->Supers_VTClassA_VTable->method4(&self->classA);
}

const struct VTClassB_VTable* VTClassB_VTable_Key( )
{
	/* Only need one vtable for every instance of this class. */
	static struct VTClassB_VTable vtable;

	/* Get a copy of the super's vtable. */
	vtable.VTClassA_VTable = *VTClassA_VTable_Key( );

	/* We are overriding method1, method2, method3, and method4 - do that now. */
	vtable.VTClassA_VTable.method1 = classBMethod1;
	vtable.VTClassA_VTable.method2 = classBMethod2;
	vtable.VTClassA_VTable.method3 = classBMethod3;
	vtable.VTClassA_VTable.method4 = classBMethod4;

	/* method2 and method4 need to call their super's implementation, provide that reference here. */
	vtable.Supers_VTClassA_VTable = VTClassA_VTable_Key( );

	/* Return pointer. */
	return &vtable;
}

void newVTClassB( struct VTClassB* self )
{
	CAssertObject(self);

	/* Call super's constructor. */
	newVTClassA(&self->classA);

	/* Map vtable. */
	CVTable(self, VTClassB_VTable_Key( ));
}

/****************************************************************************/
/* Class C																	*/
/****************************************************************************/
static int classCMethod3( struct VTClassA* self_ )
{
	/* This is VTClassC's implementation, so cast object back to type VTClassC. */
	struct VTClassC* self = CCast(self_);

	/* Calls super's (VTClassB) implementation of this method. */
	return VT_CLASSC_METHOD3 + ((struct VTClassC_VTable*) CGetVTable(self))->Supers_VTClassB_VTable->VTClassA_VTable.method3((struct VTClassA*) self);
}

static int classCMethod4( struct VTClassA* self_ )
{
	/* This is VTClassC's implementation, so cast object back to type VTClassC. */
	struct VTClassC* self = CCast(self_);

	/* Calls super's (VTClassB) implementation of this method. */
	return VT_CLASSC_METHOD4 + ((struct VTClassC_VTable*) CGetVTable(self))->Supers_VTClassB_VTable->VTClassA_VTable.method4((struct VTClassA*) self);
}

const struct VTClassC_VTable* VTClassC_VTable_Key( )
{
	/* Only need one vtable for every instance of this class. */
	static struct VTClassC_VTable vtable;

	/* Get a copy of the supers vtable. */
	vtable.VTClassB_VTable = *VTClassB_VTable_Key( );

	/* Overriding method3 and method4. */
	vtable.VTClassB_VTable.VTClassA_VTable.method3 = classCMethod3;
	vtable.VTClassB_VTable.VTClassA_VTable.method4 = classCMethod4;

	/* Need a reference to super's implementation for method3 and method4. */
	vtable.Supers_VTClassB_VTable = VTClassB_VTable_Key( );

	/* Return pointer. */
	return &vtable;
}

void newVTClassC( struct VTClassC* self )
{
	CAssertObject(self);

	/* Call super's constructor. */
	newVTClassB(&self->classB);

	/* Map vtable. */
	CVTable(self, VTClassC_VTable_Key( ));
}
