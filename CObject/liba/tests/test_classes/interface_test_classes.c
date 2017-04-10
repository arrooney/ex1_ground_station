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
 * Jun 21, 2016
 */

#include "interface_test_classes.h"

/************************************************************************/
/* Class A								*/
/************************************************************************/
/* Implementation of inherited interface method. */
static int ITInterface0_ClassA_Method0( struct ITInterface0* self_ )
{
	CAssertObject(self_);
	return IT_CLASSA_I0_METHOD0;
}

/* Implementation of inherited interface method. */
static int ITInterface0_ClassA_Method1( struct ITInterface0* self_ )
{
	CAssertObject(self_);
	return IT_CLASSA_I0_METHOD1;
}

/* Implementation of inherited interface method. */
static int ITInterface1_ClassA_Method0( struct ITInterface1* self_ )
{
	CAssertObject(self_);
	return IT_CLASSA_I1_METHOD0;
}

/* Implementation of inherited interface method. */
static int ITInterface2_ClassA_Method0( struct ITInterface2* self_ )
{
	CAssertObject(self_);
	return IT_CLASSA_I2_METHOD0;
}

/* Implementation of inherited interface method. */
static int ITInterface2_ClassA_Method1( struct ITInterface2* self_ )
{
	CAssertObject(self_);
	return IT_CLASSA_I2_METHOD1;
}

const struct ITClassA_VTable* ITClassA_VTable_Key( )
{
	/* Only need one of these for every instance of this class. */
	static struct ITClassA_VTable vtable;

	/* Get a copy of super's vtable. */
	vtable.CObject_VTable = *CObject_GetVTable( );

	/* Implement interface methods. */
	vtable.ITInterface1_VTable.ITInterface0_VTable.i0method0 = ITInterface0_ClassA_Method0;
	vtable.ITInterface1_VTable.ITInterface0_VTable.i0method1 = ITInterface0_ClassA_Method1;

	vtable.ITInterface1_VTable.i1method0 = ITInterface1_ClassA_Method0;

	vtable.ITInterface2_VTable.i2method0 = ITInterface2_ClassA_Method0;
	vtable.ITInterface2_VTable.i2method1 = ITInterface2_ClassA_Method1;

	/* Return pointer. */
	return &vtable;
}

/* Constructor. */
void newITClassA( struct ITClassA* self )
{
	CAssertObject(self);

	/* Construct super class. */
	/* This must be the first thing done in constructor. */
	CObject(&self->cobject);

	/* Map vtable. */
	/* This must be the second thing done in constructor. */
	CVTable(self, ITClassA_VTable_Key( ));
	
	/* Construct inherited interfaces */
	/* This must be third thing done in constructor. */
	CInterface(self, &self->itInterface2, &ITClassA_VTable_Key( )->ITInterface2_VTable);
	CInterface(self, &self->itInterface1, &ITClassA_VTable_Key( )->ITInterface1_VTable);
	CInterface(self, &self->itInterface1.itInterface0, &ITClassA_VTable_Key( )->ITInterface1_VTable.ITInterface0_VTable);
}


/************************************************************************/
/* Class B								*/
/************************************************************************/
/* Override this inherited method. */
static int ITInterface0_ClassB_Method0( struct ITInterface0* self_ )
{
	/* This is ITClassB's implementation, cast object to that type. */
	struct ITClassB* self = CCast(self_);

	/* Return sum of this macro plus value returned by super's implementation. */
	return IT_CLASSB_I0_METHOD0 + ((struct ITClassB_VTable*) CGetVTable(self))->Supers_ITClassA_VTable->ITInterface1_VTable.ITInterface0_VTable.i0method0(&self->classA.itInterface1.itInterface0);
}

/* Override this inherited method. */
static int ITInterface2_ClassB_Method0( struct ITInterface2* self_ )
{
	/* This is ITClassB's implementation, cast object to that type. */
	struct ITClassB* self = CCast(self_);

	/* Return sum of this macro plus value returned by super's implementation. */
	return IT_CLASSB_I2_METHOD0 + ((struct ITClassB_VTable*) CGetVTable(self))->Supers_ITClassA_VTable->ITInterface2_VTable.i2method0(&self->classA.itInterface2);
}

/* Relink this method. */
static int ITInterface0_ClassB_Method1( struct ITInterface0* self_ )
{
	CAssertObject(self_);

	return IT_CLASSB_I0_METHOD1;
}

/* Relink this method. */
static int ITInterface2_ClassB_Method1( struct ITInterface2* self_ )
{
	CAssertObject(self_);

	return IT_CLASSB_I2_METHOD1;
}

const struct ITClassB_VTable* ITClassB_VTable_Key( )
{
	/* Only need one of these for every instance of this class. */
	static struct ITClassB_VTable vtable;

	/* Get a copy of super's vtable for this class. */
	vtable.ITClassA_VTable = *ITClassA_VTable_Key( );

	/* Override these methods. */
	vtable.ITClassA_VTable.ITInterface1_VTable.ITInterface0_VTable.i0method0 = ITInterface0_ClassB_Method0;
	vtable.ITClassA_VTable.ITInterface1_VTable.ITInterface0_VTable.i0method1 = ITInterface0_ClassB_Method1;

	vtable.ITClassA_VTable.ITInterface2_VTable.i2method0 = ITInterface2_ClassB_Method0;
	vtable.ITClassA_VTable.ITInterface2_VTable.i2method1 = ITInterface2_ClassB_Method1;

	/* Keep a reference to the super's implementation of certain methods. */
	vtable.Supers_ITClassA_VTable = ITClassA_VTable_Key( );

	/* Return pointer. */
	return &vtable;
}

void newITClassB( struct ITClassB* self )
{
	CAssertObject(self);

	/* Construct super class. */
	/* Must be first thing done in constructor. */
	newITClassA(&self->classA);

	/* Map vtable. */
	/* Must be second thing done in constructor. */
	CVTable(self, ITClassB_VTable_Key( ));
}


/****************************************************************************/
/* Class C																	*/
/****************************************************************************/
/* Override this inherited method. */
static int ITInterface0_ClassC_Method0( struct ITInterface0* self_ )
{
	/* This is ITClassC's implementation, cast object to that type. */
	struct ITClassC* self = CCast(self_);

	/* Return sum of this macro plus value returned by super's implementation. */
	return IT_CLASSC_I0_METHOD0 + ((struct ITClassC_VTable*) CGetVTable(self))->Supers_ITClassB_VTable->ITClassA_VTable.ITInterface1_VTable.ITInterface0_VTable.i0method0(&self->classB.classA.itInterface1.itInterface0);
}

/* Override this inherited method. */
static int ITInterface1_ClassC_Method0( struct ITInterface1* self_ )
{
	/* This is ITClassC's implementation, cast object to that type. */
	struct ITClassC* self = CCast(self_);

	/* Return sum of this macro plus value returned by super's implementation. */
	return IT_CLASSC_I1_METHOD0 + ((struct ITClassC_VTable*) CGetVTable(self))->Supers_ITClassB_VTable->ITClassA_VTable.ITInterface1_VTable.i1method0(&self->classB.classA.itInterface1);
}

/* Override this inherited method. */
static int ITInterface2_ClassC_Method0( struct ITInterface2* self_ )
{
	/* This is ITClassC's implementation, cast object to that type. */
	struct ITClassC* self = CCast(self_);

	/* Return sum of this macro plus value returned by super's implementation. */
	return IT_CLASSC_I2_METHOD0 + ((struct ITClassC_VTable*) CGetVTable(self))->Supers_ITClassB_VTable->ITClassA_VTable.ITInterface2_VTable.i2method0(&self->classB.classA.itInterface2);
}

const struct ITClassC_VTable* ITClassC_VTable_Key( )
{
	/* Only need one of these for every instance of this class. */
	static struct ITClassC_VTable vtable;

	/* Get copy of supers vtable. */
	vtable.ITClassB_VTable = *ITClassB_VTable_Key( );
	
	/* Override these methods. */
	vtable.ITClassB_VTable.ITClassA_VTable.ITInterface1_VTable.ITInterface0_VTable.i0method0 = ITInterface0_ClassC_Method0;
	vtable.ITClassB_VTable.ITClassA_VTable.ITInterface1_VTable.i1method0 = ITInterface1_ClassC_Method0;
	vtable.ITClassB_VTable.ITClassA_VTable.ITInterface2_VTable.i2method0 = ITInterface2_ClassC_Method0;

	/* Keep reference to super's vtable. */
	vtable.Supers_ITClassB_VTable = ITClassB_VTable_Key( );

	/* Return pointer. */
	return &vtable;
}

void newITClassC( struct ITClassC* self )
{
	CAssertObject(self);

	/* Construct super class. */
	/* Must be first thing done in constructor. */
	newITClassB(&self->classB);

	/* Map vtable. */
	/* Must be second thing done in constructor. */
	CVTable(self, ITClassC_VTable_Key( ));
}
