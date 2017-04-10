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
 * July 3, 2016
 */
/**
 * @file
 */
#ifndef UTIL_CIARRAYLIST_H_
#define UTIL_CIARRAYLIST_H_

#include "CIList.h"
#include <CError.h>


/* There is one bit for every element in the list do indicate full/empty status.
 * To get the number of bytes required for a one to one
 * map of element indices to bits: divide the max size of the list by the
 * number of bits in a byte, then add one to ensure
 * a round up. 
 */
#define CCARRAY_LIST_MASK_SIZE( list_size ) (((list_size) >> 3) + 1)

/* Macro which resolves to the number of bytes that must be
 * allocated for a list of length 'list_length' with elements of size
 * 'element_length'.
 */
#define CCARRAY_LIST_SIZE( element_length, list_length )				\
	(((list_length) * (element_length)) + CCARRAY_LIST_MASK_SIZE(list_length))


/************************************************************************/
/* Class and vtable decalre.						*/
/************************************************************************/
/**
 * @struct CCArrayList
 * @extends CIList
 * @ingroup Lists
 * @see CCListIterator
 * @brief
 *	Array implementation of CIList.
 * @details
 *	Implements a non resizable, copy by value, list using an array as the underlying 
 * 	data structure. Below is an example code showing adding items to 
 *	a list which can hold chars, and has a length of 3.
 * @code
 *	struct CCArrayList list;
 *
 *	CCArrayList(&list, sizeof(unsigned char), 3);
 *
 *	unsigned char test_val = 0;
 *	CIList_Add(&list.cIList, &test_val);
 *	++test_val;
 *	CIList_Add(&list.cIList, &test_val);
 *	++test_val;
 *	CIList_Add(&list.cIList, &test_val);
 *
 *	CIList_Get(&list.cIList, &test_val, 0);
 *	printf("test val at index 0 is %d", test_val);
 *	CIList_Get(&list.cIList, &test_val, 1);
 *	printf("test val at index 1 is %d", test_val);
 *	CIList_Get(&list.cIList, &test_val, 2);
 *	printf("test val at index 2 is %d", test_val);
 *
 *	CDestroy(&list);
 * @endcode
 *	Valid indices into the array list are [0, 1, ..., n-1] where n is 
 *	the length of the list.
 *	<br>Since this is a copy by value list, all inputs to
 *	the list point to the memory location where data will be copied from/to.
 *	<br>In the above example, the list was created to copy only one byte 
 *	(the size of an unsigned char). When adding/removing elements, the
 *	address of the char was provided, because the input points to the memory
 *	location which will be copied from/to.
 * @attention
 *	- To call inherited methods from CIList, use as input to first argument:
 *		- CCArrayList::cIList
 */
struct CCArrayList
{
	/* Super class must always be first member */
	/* of a C class struct. */
	struct CObject cobject;

	/* Implementing the CIList interface. */
	struct CIList cilist;

	/* Private member variables. */
	struct
	{
		/* Base address of memory used for the lsit
		 */
		char* list_base;

		/* Defines a bit mask to indicate the empty/full
		 * status of all list elements. For example, bit 3
		 * indicates if the element at index 3 is empty/full.
		 */
		unsigned char* list_mask;
		
		size_t max_size;
		size_t current_size;
		size_t element_size;

		/* Holds the last known empty index within the list.
		 * This is a cache to speed up the CIList_Add( ) 
		 * implementation.
		 */
		size_t add_index;

		/* Used in destructor.
		 */
		CBool is_static;
	} _;
};

/**
 * @ingroup VTable
 * @brief
 *	CCArrayList's vtable declaration.
 */
struct CCArrayList_VTable
{
	/* Space for a copy of the super class' virtual table must  */
	/* be the first member of a class virtual table declaration. */
	struct CObject_VTable  cobject_override;

	/* Since we are overriding the destructor, we need to keep */
	/* keep a reference to the super class' implementation of */
	/* the destructor. */
	const struct CObject_VTable* cobject;

	/* Space for a copy of the implemented interface's virtual table */
	struct CIList_VTable cilist_override;
};

/**
 * @memberof CCArrayList
 * @ingroup VTable
 * @details
 *	Get vtable reference for CCArrayList class.	
 */
const struct CCArrayList_VTable* CCArrayList_GetVTable( );


/************************************************************************/
/* Constructor								*/
/************************************************************************/
/**
 * @memberof CCArrayList
 * @constructor
 * @details
 *	Creates an array list using CMalloc (defined in Class.h) to allocate
 *	space. The created list is not resizable and uses an array as the 
 *	back end data structure to implement the list methods. This is a 
 *	copy by value list, so all inputs to the list point to a memory
 *	location where data is copied from/to.
 * @param self
 *	The list.
 * @param element_size
 *	The size of elements being added/removed from the list.
 * @param max_size
 *	The maximum number of elements which can be in the list at one time.
 * @returns
 *	Error code. 
 *	<br>COBJ_OK: on successful construction.
 *	<br>COBJ_ALLOC_FAIL: on failure to allocate memory for list.
 */	
CError CCArrayList( struct CCArrayList* self, size_t element_size, size_t max_size );

/**
 * @memberof CCArrayList
 * @constructor
 * @details
 * 	Creates an array list using the memory block provided as input. The size
 *	of the memory block for different element_size and max_size lists
 *	can be resolved at compile time using the macro CCARRAY_LIST_SIZE().
 *	For example, to make a list with the max size of three elements, and
 *	each element being the size of an int:
 *	@code
 *		struct CCArrayList list;
 *		char list_memory[CCARRAY_LIST_SIZE(sizeof(int), 3)];
 *		CCArrayListStatic(&list, sizeof(int), 3, list_memory);
 *	@endcode
 *	Also, take a look at CCArrayList().
 * @param self
 *	The list.
 * @param element_size
 *	The size of elements being copied into/out of the list.
 * @param max_size
 *	The maximum number of elements in the list.
 * @returns 
 *	Always returns COBJ_OK.
 */	
CError CCArrayListStatic
(
	struct CCArrayList* self,
	size_t element_size,
	size_t max_size,
	void* memory
);

#endif /* UTIL_CIARRAYLIST_H_ */

