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
 * July 13, 2016
 */
/**
 * @file
 */

#ifndef UTIL_CCBINARYTREE_H_
#define UTIL_CCBINARYTREE_H_

#include <CITree.h>
#include <CCArrayList.h>

/************************************************************************/
/* Class and vtable decalre.						*/
/************************************************************************/
/**
 * @struct CCBinaryTree
 * @extends CITree
 * @ingroup Trees
 * @brief
 *	Implements the CITree interface with a binary tree/heap.
 * @details
 *	Implements the CITree interface with a binaray tree/heap. Data and keys put into
 *	the tree are copied by value. This implementation allows arbitrary
 *	keys to be used when inserting into the tree. The root of the tree is always the
 *	node with the key that evaluates to be smaller than all other keys. The evaluation
 *	function for keys is passed into the constructor as a function argument. For example,
 *	the tree below uses integers for keys, can have a maximum of 8 nodes, and will save
 *	integers in each of its nodes. The code puts an item in with value 0 and key 0, then
 *	an item with value 5 and key -3. This will put the -3:5 key:value pair at the root of
 *	tree. Next time CITree_Pop() is called, that is the item that gets removed, since its
 *	key will evaluate as being the smallest.
 * 	@code
 *		static signed char compare_function( const void* key1_p, const void* key2_p )
 *		{
 *			int key1 = *((int*) key1_p);
 *			int key2 = *((int*) key2_p);
 *
 *			if( key1 < key2 ) { 
 *				return -1; 
 *			}
 *			else if( key1 > key2 ) { 
 *				return 1; 
 *			}
 *			else { 
 *				return 0; 
 *			}
 *		}
 *
 *		int main( int argc, char** argv ) 
 *		{
 *			struct CCBinaryTree tree;
 *			CCBinaryTree(&tree
 *				     sizeof(int),
 *			  	     8,
 *				     compare_function,
 *				     sizeof(int));
 *
 *			int item = 0, key = 0;
 *			CITree_Push(&tree.cITree, &item, &key);
 *
 *			item = 5; key = -3;
 *			CITree_Push(&tree.cITree, &item, &key);
 *
 *			CDestroy(&tree);
 *		}
 *	@endcode
 */
struct CCBinaryTree
{
	/* Super class must always be first member */
	/* of a C class struct. */
	struct CObject cobject;

	/* Implementing the CITree interface. */
	struct CITree citree;

	/* Private data. */	
	struct CCArrayList tree_backend;
	unsigned char* swap_space_1;
	unsigned char* key_space_1;
	unsigned char* key_space_2;
	signed char (*compare)(const void*, const void*);
	size_t index;
	size_t element_size;
	size_t key_size;
	size_t max_size;
};

/**
 * @ingroup VTable
 * @brief
 *	CCBinaryTree's vtable declaration.
 */
struct CCBinaryTree_VTable
{
	/* Space for a copy of the super class' virtual table must  */
	/* be the first member of a class virtual table declaration. */
	struct CObject_VTable  cobject_override;

	/* Since we are overriding the destructor, we need to keep */
	/* keep a reference to the super class' implementation of */
	/* the destructor. */
	const struct CObject_VTable* cobject;

	/* Space for a copy of the implemented interface's virtual table */
	struct CITree_VTable citree_override;
};

/**
 * @memberof CCBinaryTree
 * @ingroup VTable
 * @details
 *	Get vtable reference for CCBinaryTree class.	
 */
const struct CCBinaryTree_VTable* CCBinaryTree_GetVTable( );
	

/************************************************************************/
/* Constructor								*/
/************************************************************************/
/**
 * @memberof CCBinaryTree
 * @constructor
 * @details
 *	Construct a binary tree. The tree's memory is allocated using CMalloc 
 *	defined in Class.h. When the tree is no longer needed, CDestroy() must be
 *	called on it.
 * @param self
 *	The tree.
 * @param element_size
 *	The number of bytes of data per node. This is how many bytes are copied
 *	into a node when CITree_Push() is called. This must be greater than or equal
 *	to one.
 * @param max_size
 *	The maximum number of nodes in the tree. This must be greater than or equal to
 *	2.
 * @param compare
 *	A function pointer used for comparing keys. The prototype of the function is
 * 	@code
 *		signed char compare( const void* key1, const void* key2 );
 *	@endcode
 *	The inputs point to the location of two keys in memory. This method
 *	must be return:
 *		- -1 if key1 evaluates to be less than key2.
 *		- +1 if key1 evaluates to be greater than key2.
 *		- 0 if key1 and key2 evalute to be equal.
 *	Keeping in mind, that keys which evaluate to be of lower value are moved closer
 *	to the root of the tree. An example implementation is given in the description
 *	of struct CCBinaryTree.
 * @param key_size
 * 	The size of keys in bytes. Keys are copied into the tree when CITree_Push() is called.
 * 	This must be greater than or equal to 1.
 * @returns
 *	- COBJ_OK: construction successfull.
 *	- COBJ_INV_PARAM: Input parameters are not valid. 
 *	- COBJ_ALLOC_FAIL: Failed to allocate memory for the tree using CMalloc.
 */
CError CCBinaryTree
(
	struct CCBinaryTree* self,
	size_t element_size,
	size_t max_size,
	signed char (*compare)( const void*, const void* ),
	size_t key_size
);


#endif /* UTIL_CCBINARYTREE_H_ */
