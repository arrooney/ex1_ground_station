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
 * Jun 18, 2016
 *
 * These classes are used to test:
 * 		Memory free hook in destructor is correctly called in base class A.
 *
 *		Destructor call is correctly made when B inherits from A. (B->A).
 *
 * 		Destructor calls are correctly cascaded when C overrides destructor. (C->A).
 *
 * 		Destructor calls are correctly cascaded when D overrides destructor from B. (D->B->A).
 *
 * 		Destructor calls are correctly cascaded when E overrides destructor in C. (E->C->A).
 */
#ifndef TESTS_TEST_CLASSES_H_
#define TESTS_TEST_CLASSES_H_

#include <Class.h>

#define DT_CLASS_A_VAL 0
#define DT_CLASS_E_VAL -4

/****************************************************************************/
/* Test classes A															*/
/****************************************************************************/
struct DTClassA_VTable;
struct DTClassA
{
	struct CObject cobject;

	int* destructorTestVar;
};
struct DTClassA_VTable
{
	struct CObject_VTable CObject_VTable;
};

/****************************************************************************/
/* Test class B																*/
/****************************************************************************/
struct DTClassB_VTable;
struct DTClassB
{
	struct DTClassA dtClassA;
};
struct DTClassB_VTable
{
	struct DTClassA_VTable DTClassA_VTable;
};


/****************************************************************************/
/* Test class C																*/
/****************************************************************************/
struct DTClassC_VTable;
struct DTClassC
{
	struct DTClassA dtClassA;
};

struct DTClassC_VTable
{
	struct DTClassA_VTable DTClassA_VTable;
	const struct DTClassA_VTable* Supers_DTClassA_VTable;
};

/****************************************************************************/
/* Test class D																*/
/****************************************************************************/
struct DTClassD_VTable;
struct DTClassD
{
	struct DTClassB dtClassB;
};

struct DTClassD_VTable
{
	struct DTClassB_VTable DTClassB_VTable;
	const struct DTClassB_VTable* Supers_DTClassB_VTable;
};

/****************************************************************************/
/* Test class E																*/
/****************************************************************************/
struct DTClassE_VTable;
struct DTClassE
{
	struct DTClassC dtClassC;
};
struct DTClassE_VTable
{
	struct DTClassC_VTable DTClassC_VTable;
	const struct DTClassC_VTable* Supers_DTClassC_VTable;
};


/****************************************************************************/
/* Constructors																*/
/****************************************************************************/
extern const struct DTClassA_VTable* DTClassA_VTable_Key( );
extern void newDTClassA( struct DTClassA*, int* );
extern const struct DTClassB_VTable* DTClassB_VTable_Key( );
extern void newDTClassB( struct DTClassB*, int* );
extern const struct DTClassC_VTable* DTClassC_VTable_Key( );
extern void newDTClassC( struct DTClassC*, int* );
extern const struct DTClassD_VTable* DTClassD_VTable_Key( );
extern void newDTClassD( struct DTClassD*, int* );
extern const struct DTClassE_VTable* DTClassE_VTable_Key( );
extern void newDTClassE( struct DTClassE*, int* );


#endif /* TESTS_TEST_CLASSES_H_ */
