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
 * Jun 24, 2016
 */
/**
 * @file
 * @defgroup Lists
 * All list type data structures.
 */
#ifndef UTIL_CILIST_H_
#define UTIL_CILIST_H_

#include <Class.h>
#include <stdlib.h>
#include <CIIterator.h>


/************************************************************************/
/* Error codes								*/
/************************************************************************/
/**
 * @enum CIListError
 * @ingroup Lists
 * @var CILIST_OK
 *	No error.
 * @var CILIST_ERR_FULL
 *	No room for data.
 * @var CILIST_ERR_EMPTY
 *	No data.
 * @var CILIST_ERR_INDEX
 *	Invalid index.
 */
typedef enum
{
	CILIST_OK = 0,
	CILIST_ERR_FULL = 1,
	CILIST_ERR_EMPTY = 2,
	CILIST_ERR_INDEX = 3
} CIListError;

/************************************************************************/
/* Interface declaration.						*/
/************************************************************************/
/**
 * @struct CIList
 * @ingroup Lists
 * @brief
 *	Interface for list data structures.
 * @details
 *	Provides an interface for adding/removing data from a list.
 *	This is a copy by value list, meaning all insert/get/remove functions
 *	will copy at the location pointed to by inputs. See function descriptions
 *	for more details.
 */
struct CIList
{
	/* CInterface must always be first member of */
	/* an interface's struct .*/
	struct CInterface interface;
};

/**
 * @ingroup VTable
 * @brief
 *	CIList virtual table.
 */
struct CIList_VTable
{
	CIListError (*add)( struct CIList*, void* );
	CIListError (*addAt)( struct CIList*, void*, size_t );
	CIListError (*get)( struct CIList*, void*, size_t );
	CIListError (*remove)( struct CIList*, void*, size_t );
	void (*clear)( struct CIList* );
	size_t (*size)( struct CIList* );
	size_t (*maxSize)( struct CIList* );
};


/************************************************************************/
/* Member functions.							*/
/************************************************************************/
/**
 * @memberof CIList
 * @details
 * 	Add parameter element, by copy, to the first empty spot in the list. If 
 *	there are no empty spots, the list will be resized or an error returned.
 * @param self
 *	The list.
 * @param element
 *	A pointer to the data which will be copied into the list.
 * @returns
 *	- CILIST_OK:		The element was inserted into the list
 *	- CILIST_ERR_FULL:	No room in the list, and if a resize was attempted
 *				it failed. the element wasn't added.
 */
CIListError CIList_Add( struct CIList* self, void* element );

/**
 * @memberof CIList
 * @details
 *	Insert the element into the specefied index of the list. If there is already something
 *	at that index in the list, it gets overwritten.
 * @param self
 * 	The list.
 * @param element
 *	Pointer to the data that is copied into the list.
 * @param index
 *	The index in the list to copy the data.
 * @returns
 *	- CILIST_OK:		Element was inserted.
 *	- CILIST_ERR_INDEX:	Index out of bounds.
 */
CIListError CIList_AddAt( struct CIList* self, void* element, size_t index );

/**
 * @memberof CIList
 * @details
 *	Get an element from the list at the specified index. The element is not removed
 *	from the list.
 * @param self
 *	The list.
 * @param element
 *	Data will be copied from the list into the location pointed to by this. If a NULL
 *	pointer is provided, no data is copied.
 * @param index
 *	The location in the list to get an item from.
 * @returns
 *	- CILIST_OK:		Retrieved the element.
 *	- CILIST_ERR_EMPTY:	Nothing at the location given by parameter index. 
 *	- CILIST_ERR_INDEX:	The index is out of bounds, not a valid index.
 */	
CIListError CIList_Get( struct CIList* self, void* element, size_t index );

/**
 * @memberof CIList
 * @details
 *	Remove an element from the list at the specified index.
 * @param self
 *	The list.
 * @param element
 *	Pointer to location where data removed from the list will be copied into. If this is
 *	a NULL pointer, no data is copied.
 * @param index
 *	Index in the list to remove data. 
 * @returns
 *	- CILIST_OK:		Element removed from given index.
 *	- CILIST_ERR_EMPTY:	No element at the given index.
 *	- CILIST_ERR_INDEX:	Index is out of bounds.
 */
CIListError CIList_Remove( struct CIList* self, void* element, size_t index );

/**
 * @memberof CIList
 * @details
 *	Reset the list to a completely empty state. All data
 *	within the list is lost.
 * @param self
 *	The list.
 */
void CIList_Clear( struct CIList* self);

/**
 * @memberof CIList
 * @details
 *	Poll the current size of the list.
 * @param self
 *	The list.
 * @returns
 *	Number of items currently in the list.
 */
size_t CIList_Size( struct CIList* self );

/**
 * @memberof CIList
 * @details
 *	Poll the currently maximum available space in the list.
 * @param self
 *	The list.
 * @returns
 *	The maximum available space in the list.
 */
size_t CIList_MaxSize( struct CIList* self );

#endif /* UTIL_CILIST_H_ */
