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
 *
 * These classes are used to test:
 *
 * 		Implementing an interface's methods. (A->I2).
 * 			* Can implement i2method0
 * 			* Can implement i2method1
 *
 * 		Implementing methods in an interface which are inherited from an interface. (A->I1->I0).
 * 			* Can implement i0method0
 * 			* Can implement i0method1
 * 			* Can implement i1method0
 *
 * 		Overriding and relinking inherited interface methods in B. (B->A).
 * 			* Can override i0method0
 * 			* Can relink i0method1
 * 			* Can override i2method0
 * 			* Can relink i2method1
 *
 * 		Overriding inherited interface methods in C. (C->B->A).
 * 			* Can override i0method0
 * 			* Can override i2method0
 * 			* Can override i1method0
 */
#ifndef TESTS_TEST_CLASSES_INTERFACE_TEST_CLASSES_H_
#define TESTS_TEST_CLASSES_INTERFACE_TEST_CLASSES_H_

#include <Class.h>

#define IT_CLASSA_I0_METHOD0 1
#define IT_CLASSA_I0_METHOD1 2
#define IT_CLASSA_I2_METHOD0 3
#define IT_CLASSA_I2_METHOD1 4
#define IT_CLASSA_I1_METHOD0 5

#define IT_CLASSB_I0_METHOD0 6
#define IT_CLASSB_I0_METHOD1 7
#define IT_CLASSB_I2_METHOD0 8
#define IT_CLASSB_I2_METHOD1 9

#define IT_CLASSC_I0_METHOD0 10
#define IT_CLASSC_I1_METHOD0 12
#define IT_CLASSC_I2_METHOD0 11

/************************************************************************/
/* Interface 0								*/
/************************************************************************/
struct ITInterface0
{
	/* Must be first member of an interface. */
	struct CInterface interface;
};

struct ITInterface0_VTable
{
	/* Interface methods are defined in the interface's vtable. */
	int (*i0method0)( struct ITInterface0* );
	int (*i0method1)( struct ITInterface0* );
};

/* Wrapper for calling interface method. */
static inline int ITInterface0_Method0( struct ITInterface0* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct ITInterface0_VTable*) CGetVTable(self))->i0method0);
	return ((struct ITInterface0_VTable*) CGetVTable(self))->i0method0(self);
}

/* Wrapper for calling interface method. */
static inline int ITInterface0_Method1( struct ITInterface0* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct ITInterface0_VTable*) CGetVTable(self))->i0method1);
	return ((struct ITInterface0_VTable*) CGetVTable(self))->i0method1(self);
}


/************************************************************************/
/* Interface 1								*/
/************************************************************************/
struct ITInterface1
{
	/* Must be first member of an interface. */
	struct CInterface interface;

	/* Inherit from interface0. */
	struct ITInterface0 itInterface0;
};

struct ITInterface1_VTable
{
	/* Must include all super interface's vtable in this */
	/* interfaces vtable. */
	struct ITInterface0_VTable ITInterface0_VTable;

	/* This interfaces methods are defined in its vtable. */
	int (*i1method0)( struct ITInterface1* );
};

/* Wrapper for calling interface method. */
static inline int ITInterface1_Method0( struct ITInterface1* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct ITInterface1_VTable*) CGetVTable(self))->i1method0);
	return ((struct ITInterface1_VTable*) CGetVTable(self))->i1method0(self);
}


/************************************************************************/
/* Interface 2								*/
/************************************************************************/
struct ITInterface2
{
	/* Must be first member of an interface. */
	struct CInterface interface;
};

struct ITInterface2_VTable
{
	/* The interface's methods must be defined in its vtable. */
	int (*i2method0)( struct ITInterface2* );
	int (*i2method1)( struct ITInterface2* );
};

/* Wrapper for calling interface method. */
static inline int ITInterface2_Method0( struct ITInterface2* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct ITInterface2_VTable*) CGetVTable(self))->i2method0);
	return ((struct ITInterface2_VTable*) CGetVTable(self))->i2method0(self);
}

/* Wrapper for calling interface method. */
static inline int ITInterface2_Method1( struct ITInterface2* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct ITInterface2_VTable*) CGetVTable(self))->i2method1);
	return ((struct ITInterface2_VTable*) CGetVTable(self))->i2method1(self);
}


/************************************************************************/
/* Class A								*/
/************************************************************************/
struct ITClassA
{
	/* Super class must be first member of the class declaration. */
	struct CObject cobject;

	/* Inherit from these interfaces */
	struct ITInterface2 itInterface2;
	struct ITInterface1 itInterface1;
};

struct ITClassA_VTable
{
	/* Space for a copy of the super classes vtable must */
	/* be the first member of any classes vtable. */
	struct CObject_VTable CObject_VTable;

	/* The vtables of inherited interfaces must be included as copies in this classes vtable. */
	/* However, unlike the super class' vtable, this vtables have no ordering requirement. */
	struct ITInterface1_VTable ITInterface1_VTable;
	struct ITInterface2_VTable ITInterface2_VTable;
};

/* Function to get the reference to this class' vtable. */
const struct ITClassA_VTable* ITClassA_VTable_Key( );
/* Constructor. */
void newITClassA( struct ITClassA* );


/************************************************************************/
/* Class B								*/
/************************************************************************/
struct ITClassB
{
	/* Super class must be first member of class declaration. */
	struct ITClassA classA;
};

struct ITClassB_VTable
{
	/* Space for a copy of the super class' vtable must */
	/* be the first member of this class' vtable declaration. */
	struct ITClassA_VTable ITClassA_VTable;

	/* We are overriding virtual methods inherited from the super class. In */
	/* the new method definition, we want to call the super implementation. */
	/* To do this, we need a reference to the super class' vtable. */
	const struct ITClassA_VTable* Supers_ITClassA_VTable;
};

/* Used to get a reference to this class' vtable. */
const struct ITClassB_VTable* ITClassB_VTable_Key( );	
/* Constructor. */
void newITClassB( struct ITClassB* );


/************************************************************************/
/* Class C								*/
/************************************************************************/
struct ITClassC
{
	/* Super class must be first member of class declaration. */
	struct ITClassB classB;
};

struct ITClassC_VTable
{
	/* Space for a copy of super class' vtable must be first member */
	/* of any classes vtable declaration. */
	struct ITClassB_VTable ITClassB_VTable;

	/* We are overriding virtual methods inherited from the super class. In */
	/* the new method definition, we want to call the super implementation. */
	/* To do this, we need a reference to the super class' vtable. */
	const struct ITClassB_VTable* Supers_ITClassB_VTable;
};

/* Used to get a reference to this class' vtable. */
const struct ITClassC_VTable* ITClassC_VTable_Key( );
/* Constructor. */
void newITClassC( struct ITClassC* );

#endif /* TESTS_TEST_CLASSES_INTERFACE_TEST_CLASSES_H_ */
