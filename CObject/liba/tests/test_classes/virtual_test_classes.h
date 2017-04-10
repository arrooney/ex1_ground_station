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
 * These are classes to test:
 * 		Linking and calling virtual methods from class A.
 * 				Done using: method0
 *
 * 		Inheriting and calling virtual methods in B that were inherited from A. (B->A).
 * 				Done using: method0
 *
 * 		Relinking virtual methods in class B that were defined in A. (B->A).
 * 				Done using: method1
 *
 * 		Overriding virtual method in class B that was defined in A. (B->A).
 * 				Done using: method2
 *
 * 		Overriding virtual method in class C which was relinked in class B and defined in class A. (C->B->A).
 * 				Done using: method3
 *
 * 		Overriding virtual method in class C which was overrode in class B and defined in class A. (C->B->A).
 * 				Done using: method4
 */
#ifndef TESTS_TEST_CLASSES_VIRTUAL_TEST_CLASSES_H_
#define TESTS_TEST_CLASSES_VIRTUAL_TEST_CLASSES_H_

#include <Class.h>

#define VT_CLASSA_METHOD0 1
#define VT_CLASSA_METHOD1 2
#define VT_CLASSA_METHOD2 3
#define VT_CLASSA_METHOD3 4
#define VT_CLASSA_METHOD4 5

#define VT_CLASSB_METHOD1 9
#define VT_CLASSB_METHOD2 6
#define VT_CLASSB_METHOD3 10
#define VT_CLASSB_METHOD4 7

#define VT_CLASSC_METHOD3 11
#define VT_CLASSC_METHOD4 8

/************************************************************************/
/* Class A								*/
/************************************************************************/
struct VTClassA
{
	struct CObject cobject;
};

struct VTClassA_VTable
{
	struct CObject_VTable CObject_VTable;
	
	int (*method0)( struct VTClassA* );
	int (*method1)( struct VTClassA* );
	int (*method2)( struct VTClassA* );
	int (*method3)( struct VTClassA* );
	int (*method4)( struct VTClassA* );
};

const struct VTClassA_VTable* VTClassA_VTable_Key( );
void newVTClassA( struct VTClassA* );

static inline int VTClassA_Method0( struct VTClassA* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct VTClassA_VTable*) CGetVTable(self))->method0);
	return ((struct VTClassA_VTable*) CGetVTable(self))->method0(self);
}

static inline int VTClassA_Method1( struct VTClassA* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct VTClassA_VTable*) CGetVTable(self))->method1);
	return ((struct VTClassA_VTable*) CGetVTable(self))->method1(self);
}

static inline int VTClassA_Method2( struct VTClassA* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct VTClassA_VTable*) CGetVTable(self))->method2);
	return ((struct VTClassA_VTable*) CGetVTable(self))->method2(self);
}

static inline int VTClassA_Method3( struct VTClassA* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct VTClassA_VTable*) CGetVTable(self))->method3);
	return ((struct VTClassA_VTable*) CGetVTable(self))->method3(self);
}

static inline int VTClassA_Method4( struct VTClassA* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct VTClassA_VTable*) CGetVTable(self))->method4);
	return ((struct VTClassA_VTable*) CGetVTable(self))->method4(self);
}


/****************************************************************************/
/* Class B																	*/
/****************************************************************************/
struct VTClassB
{
	struct VTClassA classA;
};

struct VTClassB_VTable
{
	struct VTClassA_VTable VTClassA_VTable;
	const struct VTClassA_VTable* Supers_VTClassA_VTable;
};

const struct VTClassB_VTable* VTClassB_VTable_Key( );
void newVTClassB( struct VTClassB* );

/****************************************************************************/
/* Class C																	*/
/****************************************************************************/
struct VTClassC
{
	struct VTClassB classB;

	int (*method3)( struct VTClassA* );
	int (*method4)( struct VTClassA* );
};

struct VTClassC_VTable
{
	struct VTClassB_VTable VTClassB_VTable;
	const struct VTClassB_VTable* Supers_VTClassB_VTable;
};

const struct VTClassC_VTable* VTClassC_VTable_Key( );
void newVTClassC( struct VTClassC* );

#endif /* TESTS_TEST_CLASSES_VIRTUAL_TEST_CLASSES_H_ */
