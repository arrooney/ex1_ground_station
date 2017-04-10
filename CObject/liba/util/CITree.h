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
 * July 12, 2016
 */
/**
 * @file
 * @defgroup Trees
 * All tree data structures.
 */
#ifndef UTIL_CITREE_H_
#define UTIL_CITREE_H_

#include <Class.h>

/************************************************************************/
/* Error codes.								*/
/************************************************************************/
/**
 * @enum CITreeError
 * @ingroup Trees
 * @var CITREE_OK
 *	No error.
 * @var CITREE_ERR_EMPTY
 *	No data in the tree.
 * @var CITREE_ERR_FULL
 *	No room in tree for more data.
 * @var CITREE_ERR_EXT
 *	Transient error in helper objects. 
 */
typedef enum
{
	CITREE_OK = 0,
	CITREE_ERR_EMPTY = 1,
	CITREE_ERR_FULL = 2,
	CITREE_ERR_EXT = 3
} CITreeError;


/************************************************************************/
/* Declare interface and vtable.					*/
/************************************************************************/
/**
 * @struct CITree
 * @ingroup Trees
 * @brief
 *	Tree data structure interface
 * @details
 *	An interface for adding/removing data from trees. The tree
 *	is copy by value, meaning data and keys added to the tree are copied
 *	into it.
 */
struct CITree
{
	/* CInterface must always be first member of */
	/* an interfaces struct .*/
	struct CInterface interface;
};

/**
 * @ingroup VTable
 * @brief
 *	CITree virtual table.
 */
struct CITree_VTable
{
	CITreeError (*push)( struct CITree*, const void*, const void* );
	CITreeError (*pop)( struct CITree*, void* );
	CITreeError (*peek)( struct CITree*, void* );
	CITreeError (*get)( struct CITree*, void*, size_t );
	CITreeError (*delete)( struct CITree*, void*, size_t );
	CITreeError (*delete_element)( struct CITree*, void* );
	size_t (*size)( struct CITree* );
	size_t (*max_size)( struct CITree* );
	void (*clear)( struct CITree* );

	/* Reserved for future expansion.
	 */
	CITreeError (*split)( struct CITree*, void* );

	/* Reserved for future expansion.
	 */
	CITreeError (*merge)( struct CITree*, struct CITree* );
};


/************************************************************************/
/* Class methods. 							*/
/************************************************************************/
/**
 * @memberof CITree
 * @details
 *	Put an element into the tree by copy and move it up the tree
 *	such that the heap property is not violated using the input param
 *	key to evaluate the heap property. The heap property requires the
 *	root of the tree has the key which evaluates to be less than
 *	all other keys in the tree.
 * @param self
 *	The tree.
 * @param element
 *	Pointer to data that will be copied into the three.
 * @param key
 *	Pointer to the key for the data added to the tree.
 * @returns
 *	- CITREE_ERR_FULL: No room in tree to insert the element.
 *	- CITREE_OK: Element successfully inserted.	
 */ 
CITreeError CITree_Push( struct CITree* self, const void* element, const void* key );

/**
 * @memberof CITree
 * @details
 *	Get the root of the tree and remove it. The tree will then have its nodes
 *	adjusted such that the heap property is maintained.
 * @param self
 *	The tree.
 * @param element
 *	Pointer to location where that data at the root of the tree will be 
 *	copied too. Use NULL to safely ignore.
 * @returns
 *	- CITREE_ERR_EMPTY: Nothing in the tree to pop.
 *	- CITREE_OK: Root of the tree successfully popped.
 */
CITreeError CITree_Pop( struct CITree* self, void* element );

/**
 * @memberof CITree
 * @details
 *	Delete the node in the three who's element matches the input
 *	parameter element. This function has O(n) execution time.
 * @param self
 *	The tree.
 * @param element
 *	The data pointed to by this is the search critera. The function will
 *	search for a node whos element matches the data pointed to by this
 *	and remove it.
 * @returns
 *	- CITREE_ERR_EMPTY: Nothing in the tree to delete or no matching element.
 *	- CITREE_OK: Root of the tree successfully popped.
 */
CITreeError CITree_DeleteElement( struct CITree* self, void* element );

/**
 * @memberof CITree
 * @details
 * 	Get the element at the root of the tree without removing it.
 * @param self
 *	The tree.
 * @param element
 *	A pointer to the location where data will be copied out of the tree.
 * @returns
 *	- CITREE_ERR_EMPTY: Nothing in the tree to peek at.
 *	- CITREE_OK: Root of the tree successfully looked at.
 */
CITreeError CITree_Peek( struct CITree* self, void* element );

/**
 * @memberof CITree
 * @details
 *	Look at an arbitrary index in the tree. Indexing is done such that
 *	at any index i, left_child(i) = 2i + 1 and right_child(i) = 2i + 2.
 * @param element
 *	Pointer to location where data at the index will be copied to.
 * @param index
 *	Location in the tree to copy data from
 * @returns
 *	- CITREE_ERR_EMPTY: Nothing at given index to copy.
 *	- CITREE_OK: Success.
 */
CITreeError CITree_Get( struct CITree* self, void* element, size_t index );

/**
 * @memberof CITree
 * @details
 *	Delete at an arbitrary index in the tree. Indexing is done such that
 *	at any index i, left_child(i) = 2i and right_child(i) = 2i + 1.
 * @param element
 *	Pointer to location where data at the index will be copied to. Use
 *	NULL to ignore and simply delete from the tree.
 * @param index
 *	Location in the tree to delete data from
 * @returns
 *	- CITREE_ERR_EMPTY: Nothing at given index to delete.
 *	- CITREE_OK: Success.
 */
CITreeError CITree_Delete( struct CITree* self, void* element, size_t index );

/**
 * @memberof CITree
 * @details
 *	Get the current size of tree, ie, how many nodes have been
 *	added to the tree.
 * @param self
 *	The tree.
 * @returns
 *	The current size of the tree.
 */
size_t CITree_Size( struct CITree* self );

/**
 * @memberof CITree
 * @details
 *	The maximum number of nodes which can be added to the tree.
 * @param self
 *	The tree.
 * @returns
 *	The maximum number of nodes which can be added to the tree.
 */
size_t CITree_MaxSize( struct CITree* self );

/**
 * @memberof CITree
 * @details
 *	Reset the tree to have zero ndoes in it. All data in the tree is lost.
 * @param self
 *	The tree.
 */
void CITree_Clear( struct CITree* self );

#endif /* UTIL_CITREE_H_ */
