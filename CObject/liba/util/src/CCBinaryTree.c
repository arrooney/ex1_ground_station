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

#include <CCBinaryTree.h>
#include <CCListIterator.h>
#include <string.h>

#define CCBINARY_TREE_ROOT 0

/* The parents index is floor(index/2) where index=1 is used for the
 * first element in the array. To calculate this with index=0 being
 * the first element, we do floor((index+1)/2) - 1.
 */
#define PARENTS_INDEX( index ) ((((index) + 1) >> 1) - 1)
#define LCHILD_INDEX( index ) ((((index) + 1) << 1 ) - 1)
#define RCHILD_INDEX( index ) (((index) + 1) << 1 )

/************************************************************************/
/* Private methods.							*/
/************************************************************************/
/* Copy the data pointed to by element and key into swap_space_1.
 */
static void CCBinaryTree_EncodeSwap( struct CCBinaryTree* self, const void* element, const void* key )
{
	CAssertObject(self);
	memcpy(self->swap_space_1, element, self->element_size);
	memcpy(self->swap_space_1 + self->element_size, key, self->key_size);
}

/* Copy the data in swap_space_1 into element and key. These can be passed
 * in as NULL to ignore them.
 */
static void CCBinaryTree_DecodeSwap( struct CCBinaryTree* self, void* element, void* key )
{
	CAssertObject(self);
	if( element != NULL ) {
		memcpy(element, self->swap_space_1, self->element_size);
	}
	if( key != NULL ) {
		memcpy(key, self->swap_space_1 + self->element_size, self->key_size);
	}
}

/* Cache the key for the element at index. swap_space_1 will be corrupt. key_space will
 * point to the key.
 */
static void CCBinaryTree_CacheKey( struct CCBinaryTree* self, size_t index, unsigned char* key_space )
{
	CAssertObject(self);
	
	CIList_Get(&self->tree_backend.cilist, self->swap_space_1, index);
	CCBinaryTree_DecodeSwap(self, NULL, key_space);
}

/* Swap the elements at indexA and indexB. swap_space_1 is corrupt
 * by this.
 */
static void CCBinaryTree_SwapElements( struct CCBinaryTree* self, size_t indexA, size_t indexB )
{
	CAssertObject(self);

	unsigned char* swap_space_2 = self->swap_space_1 + self->element_size + self->key_size;
	
	/* Read out elements at index A and B.
	 */
	CIList_Remove(&self->tree_backend.cilist, self->swap_space_1, indexA);
	CIList_Remove(&self->tree_backend.cilist, swap_space_2, indexB);

	/* Write back in different order.
	 */
	CIList_AddAt(&self->tree_backend.cilist, swap_space_2, indexA);
	CIList_AddAt(&self->tree_backend.cilist, self->swap_space_1, indexB);
}

/* Move a node up the heap to satisfy the heap property.
 * Cyclic complexity map:
 * 1(assert) +-> 2(for) -> 3(if(root)) +-> 4(return) <--------------+
 *           |                         |                            |
 *           |                         +-> 5(cache) -> 6(if(go up)) +-> 7(swap) -+
 *           |                                                                   |
 *           +-------------------------------------------------------------------+
 *
 * This gives a complexity of: 8(edges) - 7(nodes) + 2 = 3
 */
static void CCBinaryTree_HeapifyUp( struct CCBinaryTree* self, size_t index )
{
	CAssertObject(self);
	
	for( ;; ) {
		if( index == CCBINARY_TREE_ROOT ) {
			/* This is the root node, nothing to heapify up to.
			 */
			break;
		}

		/* Cache the keys. 
		 */
		CCBinaryTree_CacheKey(self, index, self->key_space_1);
		CCBinaryTree_CacheKey(self, PARENTS_INDEX(index), self->key_space_2);

		/* Compare the keys and swap if needed.
		 */
		if( self->compare(self->key_space_1, self->key_space_2) < 0 ) {			
			/* space1 < space2 => child key < parent's key. 
			 * Need to swap with parent.
			 */
			CCBinaryTree_SwapElements(self, index, PARENTS_INDEX(index));
			index = PARENTS_INDEX(index);
		}
		else {
			/* No need to swap anymore, heap property is satisfied.
			 */
			break;
		}	
	}
}

/* Move a node down the heap to satisfy the heap property. 
 * Cyclic complexity map:
 * 1(CCast) +-> 2(for) -> 3(if(no child)) +-> 4(return) <-+
 *          |                             |               |
 *          |    +- 5(if(two children)) <-+               |
 *          |    |                                        |
 *          |    +--> 6(find smallest) --+                |
 *          |    |                       |                |
 *          |    +--> 7(left child) --+  |                |
 *          |                         |  |                |
 *          |      +------------------+--+                |
 *          |      |                                      |
 *          |      +-> 8(if(compare)) -+------------------+
 *          |                          |
 *          +--------------- 9(swap) <-+ 
 *
 * complexity of: 11(edges) - 9(nodes) + 2 = 4
 */
static void CCBinaryTree_HeapifyDown( struct CCBinaryTree* self, size_t index )
{
	CAssertObject(self);

	size_t child_index;

	for( ;; ) {
		if( LCHILD_INDEX(index) > self->index - 1 ) {
			/* No children to swap with.
			 */
			break;
		}
		else if( LCHILD_INDEX(index) < self->index - 1 ) {
			/* Find the child with the smallest key. 
			 */
			CCBinaryTree_CacheKey(self, LCHILD_INDEX(index), self->key_space_1);
			CCBinaryTree_CacheKey(self, RCHILD_INDEX(index), self->key_space_2);
			if( self->compare(self->key_space_1, self->key_space_2) < 0 ) {
				/* space1 < space 2 => lchild key < rchild key.
				 */
				child_index = LCHILD_INDEX(index);
			}
			else {
				child_index = RCHILD_INDEX(index);
			}
		}
		else {
			/* The is only a left child, no right child.
			 */
			child_index = LCHILD_INDEX(index);
		}		
		
		CCBinaryTree_CacheKey(self, child_index, self->key_space_1);
		CCBinaryTree_CacheKey(self, index, self->key_space_2);
		if( self->compare(self->key_space_1, self->key_space_2)  < 0 ) {
			/* space1 < space2 => child index < parent index.
			 * Need to swap child and parent.
			 */
			CCBinaryTree_SwapElements(self, child_index, index);
			index = child_index;
		}
		else {
			/* There is no need to swap. Heap property is satisfied.
			 */
			break;
		}
	}
}


/************************************************************************/
/* Virtual methods.							*/
/************************************************************************/
/* Cyclic complexity map:
 * 1(CCast) -> 2(if full) +-> 3(return) <-+
 *                        |               |
 *                        +-> 4(heapify) -+
 * The heapify has a cyclic complexity of 3.
 * Complexity of: 4(edges) - 4(nodes) + (3-1)(heapify) + 2 = 4
 *
 * Test cases:
 *	* Tree is full - get error code CITREE_ERR_FULL
 *	* Tree is empty - no error, no inifinite loop
 *	* New key gets heapified up to root - no error, no infinite loop
 *	* New key gets heapified up at least once, but does not replace the root - no error, no infinite loop
 */
static CITreeError CITree_Push_Def( struct CITree* self_, const void* element, const void* key )
{
	CAssertObject(self_);
	struct CCBinaryTree* self = CCast(self_);

	/* Copy the element and the key into the swap space.
	 */
	CCBinaryTree_EncodeSwap(self, element, key);

	/* Put the combined element/key pair into the list. 
	 */
	CIListError err = CIList_AddAt(&self->tree_backend.cilist, self->swap_space_1, self->index);
	if( err == CILIST_ERR_INDEX ) {
		return CITREE_ERR_FULL;
	}

	/* Move the new element up the heap such that the heap
	 * property remains satisfied.
	 */
	CCBinaryTree_HeapifyUp(self, self->index);

	/* Increment index so that this function will insert into the
	 * next element of the list when its called again.
	 */
	++self->index;

	return CITREE_OK;
}

/* Simply calls CITree_Delete with the root node as the index to
 * delete.
 */
static CITreeError CITree_Pop_Def( struct CITree* self_, void* element )
{
	CAssertObject(self_);
	struct CCBinaryTree* self = CCast(self_);

	return CITree_Delete(&self->citree, element, CCBINARY_TREE_ROOT);
}

/* Simply calls CITree_Get with the root node as the index to
 * get at.
 */ 
static CITreeError CITree_Peek_Def( struct CITree* self_, void* element )
{
	CAssertObject(self_);
	struct CCBinaryTree* self = CCast(self_);

	return CITree_Get(&self->citree, element, CCBINARY_TREE_ROOT);
}

/* Cyclic complexity map:
 * 1(CCast) -> 2(if(empty)) +-> 3(return) <-+
 *                          |               |
 *                          +-> 4(decode) --+
 * 
 * Complexity of: 4(edges) - 4(nodes) + 2 = 2
 * test cases:
 *	* tree is empty - returns error code CITREE_ERR_EMPTY
 *	* tree is not empty - gets what is in the tree.
 */
static CITreeError CITree_Get_Def( struct CITree* self_, void* element, size_t index )
{
	CAssertObject(self_);
	struct CCBinaryTree* self = CCast(self_);

	/* Get the element of the heap given by index.
	 */
	CIListError err = CIList_Get(&self->tree_backend.cilist, self->swap_space_1, index);
	if( err == CILIST_ERR_EMPTY ) {
		return CITREE_ERR_EMPTY;
	}

	/* Copy the data into the provided input. 
	 */
	CCBinaryTree_DecodeSwap(self, element, NULL);
	
	return CITREE_OK;
}

/* Cyclic complexity map:
 * 1(CCast) -> 2(if(empty)) +-> 3(return) <-+--+
 *                          |               |  |
 *           +- 4(decode) <-+               |  |
 *           |                              |  |
 *           +-> 5(if(empty)) +-> 6(reset) -+  |
 *                            |                |
 *                            +-> 7(heapify) --+
 *
 * heafiy has a complexity of 4
 * This gives complexity of: 8(edges) - 7(nodes) + (4-1)(heapify) + 2 = 6
 *
 * Test cases:
 *	* tree is empty - returns CITREE_ERR_EMPTY
 *	* tree had only one element - returns CITREE_OK and tre is now empty
 *	* Heapify ends with the heapified node having one child
 *	* Heapify ends with the heapified node having two children
 *	* Heapify ends with the heapified node having no children and no sibling
 *	* Heapify ends with the heapified node having no children and one sibling
 */
static CITreeError CITree_Delete_Def( struct CITree* self_, void* element, size_t index )
{
	CAssertObject(self_);
	struct CCBinaryTree* self = CCast(self_);

	/* Get at the index of the heap.
	 */
	CIListError err = CIList_Remove(&self->tree_backend.cilist, self->swap_space_1, index);
	if( err == CILIST_ERR_EMPTY ) {
		return CITREE_ERR_EMPTY;
	}

	/* Copy the data into the provided input. 
	 */
	CCBinaryTree_DecodeSwap(self, element, NULL);
	
	/* Check if there are more elements in the heap.
	 * If there are, the heap property needs to be
	 * satisfied. If there are no other elements,
	 * then the heap property will be satisfied.
	 */
	if( self->index <= 1 ) {
		/* This was the only element in the heap.
		 */
		self->index = CCBINARY_TREE_ROOT;
	}
	else {
		/* Place the element at the end of tree into the removed index.
		 */
		--self->index;
		err = CIList_Remove(&self->tree_backend.cilist, self->swap_space_1, self->index);
		err = CIList_AddAt(&self->tree_backend.cilist, self->swap_space_1, index);
		CCBinaryTree_HeapifyDown(self, index);
	}

	return CITREE_OK;
}

/* Cyclic complexity map:
 * 1(CCast) -> 2if(constructed iterator) +-> 3(return) <-+
 *                                       |               |
 *        +--+- 4(while not empty) < ----+               |
 *        |  |                           |               |
 *        |  +->5(found element) +-------+               |
 *        |                      |                       |
 *        |                      +-> 6(remove it) -------+
 *        -----------------------------------------------+
 *
 * This gives a complexity of:
 * 8(edges) - 6(nodes) + 2 = 4
 *
 * test cases:
 *	* Iterator fails to construct (impossible to test)
 *	* Tree is empty
 *	* No match
 *	* found a match
 */
static CITreeError CITree_DeleteElement_Def( struct CITree* self_, void* element )
{
	struct CCListIterator iter;
	size_t index;
	
	CAssertObject(self_);
	struct CCBinaryTree* self = CCast(self_);

	if( CCListIterator(&iter, &self->tree_backend.cilist) != COBJ_OK ) {
		return CITREE_ERR_EXT;
	}

	/* Look through each node of tree for matching element
	 */
	while( CIIterator_HasNext(&iter.ciiterator) ) {
		CIIterator_Next(&iter.ciiterator, self->swap_space_1);
	        if( memcmp(self->swap_space_1, element, self->element_size) == 0 ) {
			/* Found the element, remove it!
			 */
			index = CIIterator_Index(&iter.ciiterator);
			CITree_Delete(&self->citree, NULL, index);
			CDestroy(&iter);
			return CITREE_OK;
		}
	}

	CDestroy(&iter);
	return CITREE_ERR_EMPTY;
}

static size_t CITree_Size_Def( struct CITree* self_ )
{
	struct CCBinaryTree* self = CCast(self_);
	return self->index;
}

static size_t CITree_MaxSize_Def( struct CITree* self_ )
{
	struct CCBinaryTree* self = CCast(self_);
	return self->max_size;
}

static void CITree_Clear_Def( struct CITree* self_ )
{
	struct CCBinaryTree* self = CCast(self_);

	self->index = CCBINARY_TREE_ROOT;
	CIList_Clear(&self->tree_backend.cilist);
}

/************************************************************************/
/* Overriding 								*/
/************************************************************************/
static void CDestructor( void* self_ )
{
	struct CCBinaryTree* self = CCast(self_);

	CDestroy(&self->tree_backend);
	CFree(self->swap_space_1);

	/* Call super's destructor
	 */
	const struct CCBinaryTree_VTable* vtable = CGetVTable(self);
	vtable->cobject->CDestructor(self);
}


/************************************************************************/
/* vtable key								*/
/************************************************************************/
const struct CCBinaryTree_VTable* CCBinaryTree_GetVTable( )
{
	static struct CCBinaryTree_VTable vtable  =
		{
			.citree_override.push = CITree_Push_Def,
			.citree_override.pop = CITree_Pop_Def,
			.citree_override.peek = CITree_Peek_Def,
			.citree_override.get = CITree_Get_Def,
			.citree_override.delete = CITree_Delete_Def,
			.citree_override.delete_element = CITree_DeleteElement_Def,
			.citree_override.size = CITree_Size_Def,
			.citree_override.max_size = CITree_MaxSize_Def,
			.citree_override.clear = CITree_Clear_Def,
			.citree_override.split = NULL,
			.citree_override.merge = NULL
		};

	/* Super's vtable copy. */
	vtable.cobject_override = *CObject_GetVTable( );

	/* Override destructor. */
	vtable.cobject_override.CDestructor = CDestructor;

	/* Reference to super's vtable. */
	vtable.cobject = CObject_GetVTable( );

	/* Return pointer to CCArrayList's vtable. */
	return &vtable;
}



/************************************************************************/
/* Constructor								*/
/************************************************************************/
CError CCBinaryTree
(
	struct CCBinaryTree* self,
	size_t element_size,
	size_t max_size,
	signed char (*compare)( const void*, const void* ),
	size_t key_size
)
{
	CAssertObject(self);

	/* First thing in constructor must be to call super's constructor. 
	 */
	CObject(&self->cobject);

	/* Second thing in constructor must be to map vtable. 
	 */
	CVTable(self, CCBinaryTree_GetVTable( ));

	/* Third thing in constructor must be calling interface's constructor. 
	 */
	CInterface(self, &self->citree, &CCBinaryTree_GetVTable( )->citree_override);

	/* Assign the compare method. 
	 */
	if( compare == NULL ) {
		return COBJ_INV_PARAM;
	}
	self->compare = compare;

	/* check boundary conditions on element, key, and max size.
	 */
	if( element_size < 1 || key_size < 1 || max_size < 2 ) {
		return COBJ_INV_PARAM;
	}

	/* Allocate the swap space.
	 */
	self->swap_space_1 = CMalloc((2*(element_size + key_size)) + (2*key_size));
	if( self->swap_space_1 == NULL ) {
		return COBJ_ALLOC_FAIL;
	}
	self->key_space_1 = self->swap_space_1 + (2*(element_size + key_size));
	self->key_space_2 = self->key_space_1 + key_size;
	
	/* Construct the backend of the tree.
	 */
	CError err = CCArrayList(&self->tree_backend, element_size + key_size, max_size);
	if( err != COBJ_OK ) {
		CFree(self->swap_space_1);
		return err;
	}

	/* Assign remaining member functions.
	 */
	self->index = CCBINARY_TREE_ROOT;
	self->element_size = element_size;
	self->key_size = key_size;
	self->max_size = max_size;

	/* Done.
	 */
	return COBJ_OK;
}
