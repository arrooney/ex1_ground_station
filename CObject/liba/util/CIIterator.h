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
 * July 7, 2016
 */
/**
 * @file
 * @defgroup Iterators
 * All list type data structures.
 */
#ifndef UTIL_CIITERATOR_H_
#define UTIL_CIITERATOR_H_

#include <Class.h>

/************************************************************************/
/* Interface declaration.						*/
/************************************************************************/
/**
 * @struct CIIterator
 * @ingroup Iterators
 * @details
 *	Defines an interface for traversing collections of data and modifying
 *	the list while traversing. An interator always starts with its cursor
 *	positioned before the first index in a collection. Calls to CIIterator_Next()
 *	and CIIterator_Previous() move the cursor through the collection one element at a time.
 * 	Empty elements in the collection are skipped. After a call to CIIterator_Next() or
 *	CIIterator_Previous(), the cursor is put between the two elements. For example:
 *@verbatim
  element 0     element 1     element 2     element 3     ...     element n-1
^            ^             ^             ^             ^       ^               ^
  @endverbatim
 *	'^' are the possible cursor positions.
 *	<br>Below is an example of using an iterator with the CIList structure. Note, 
 *	this example assumes the iterator passed into the function has elements that are ascii
 *	strings. Since the iterator structure is copy by value, it's very important
 *	to know how large the iterator's elements are when iterating over them.
 *	@code
 *		void printListElements( struct CIIterator* iter, size_t element_size ) 
 *		{
 *			char* element = malloc(element_size);
 *			while( CIIterator_HasNext(iter) ) {
 *				CIIterator_Next(iter, element);
 *				printf("Got element: %.*s\n", element_size, element);
 *			}
 *		}
 *	@endcode
 *
 */
struct CIIterator
{
	/* CInterface must always be first member of */
	/* an interface's struct .*/
	struct CInterface interface;
};

/**
 * @ingroup VTable
 * @brief
 *	CIIterator virtual table.
 */
struct CIIterator_VTable
{
	CBool (*hasNext)( struct CIIterator* );
	void (*next)( struct CIIterator*, void* );
	CBool (*hasPrevious)( struct CIIterator* );
	void (*previous)( struct CIIterator*, void* );
	CBool (*set)( struct CIIterator*, void* );
	CBool (*remove)( struct CIIterator* );
	size_t (*index)( struct CIIterator* );
};


/************************************************************************/
/* Method declarations. 						*/
/************************************************************************/
/**
 * @memberof CIIterator
 * @details
 *	Evaluates if there is a next element in the collection. If this
 *	returns a logical true, then CIIterator_Next() will get the
 *	next element in the collection. 
 * @param self
 *	The iterator.
 * @returns
 *	- logical true (1): CIIterator_Next() will get another element in the collection.
 *	- logical false (0): At the end of the list. CIIterator_Next() will get nothing.
 */
CBool CIIterator_HasNext( struct CIIterator* self );

/**
 * @memberof CIIterator
 * @details
 *	Gets the next element in the collection. If CIIterator_HasNext() returns
 *	a logical true, then this function will get a valid element. If CIIterator_HasNext()
 * 	returned a logical false, then this function does nothing. This does not happen
 *	because of the call to CIIterator_HasNext(), CIIterator_HasNext() will only tell 
 *	the application how this method is going to behave.
 * @param self
 *	The iterator
 * @param element
 *	The next element in the collection will be copied to the the memory pointed to by this.
 */	
void CIIterator_Next( struct CIIterator* self, void* element );

/**
 * @memberof CIIterator
 * @details
 *	Evaluates if there is a previous element in the collection. If this
 *	returns a logical true, then CIIterator_Previous() will get the
 *	previous element in the collection. 
 * @param self
 *	The iterator.
 * @returns
 *	- logical true (1): CIIterator_Previous() will get another element in the collection.
 *	- logical false (0): At the end of the collection. CIIterator_Previous() will get nothing.
 */
CBool CIIterator_HasPrevious( struct CIIterator* self );

/**
 * @memberof CIIterator
 * @details
 *	Gets the previous element in the collection. If CIIterator_HasPrevious() returns
 *	a logical true, then this function will get a valid element. If CIIterator_HasPrevious()
 * 	returned a logical false, then this function does nothing.
 * @param self
 *	The iterator
 * @param element
 *	The previous element in the collection will be copied to the the memory pointed to by this.
 */
void CIIterator_Previous( struct CIIterator* self, void* element );

/**
 * @memberof CIIterator
 * @details
 *	Sets the element in the collection last returned by a call to CIIterator_Next() or 
 *	CIIterator_Previous(). If no call to those methods has been made yet, then this 
 *	does nothing and returns a logical false.
 * @param self
 *	The iterator.
 * @param element
 *	Memory pointed to will be copied into the collection
 * @returns
 *	- locical true (1): The element in the collection was set.
 *	- logical false (0): Could not set the element because no call to
 *				CIIterator_Next() or CIIterator_Previous() 
 *				has been made.
 */
CBool CIIterator_Set( struct CIIterator* self, void* element );

/**
 * @memberof CIIterator
 * @details
 *	Removes the element in the collection last returned by a call to CIIterator_Next() or 
 *	CIIterator_Previous(). If no call to those methods has been made yet, then this 
 *	does nothing and returns a logical false.
 * @param self
 *	The iterator.
 * @returns
 *	- locical true (1): The element in the collection was remove.
 *	- logical false (0): Could not remove an element because no call to
 *				CIIterator_Next() or CIIterator_Previous() 
 *				has been made.
 */
CBool CIIterator_Remove( struct CIIterator* self );

/**
 * @memberof CIIterator
 * @details
 *	Gets the index of the element in the collection last returned by a call to CIIterator_Next() or 
 *	CIIterator_Previous(). If no call to those methods has been made yet, this 
 *	does nothing and returns zero. Note, zero can be a valid index, so a returned value
 *	of zero is not an error code.
 * @param self
 *	The iterator.
 * @returns
 *	The index in the collection of the last element returned by a call to
 *	CIIterator_Next() or CIIterator_Previous().
 */
size_t CIIterator_Index( struct CIIterator* self );


#endif /* UTIL_CIITERATOR_H_ */
