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
 * July 6, 2016
 */
/**
 * @file
 */
#include <CCListIterator.h>
#include <CIList.h>

/************************************************************************/
/* Defines 								*/
/************************************************************************/

/************************************************************************/
/* Virtual methods.							*/
/************************************************************************/
 /* complexity map:
 *                    ----------
 *                    |        |
 * 1(CCast) -> 2(for) -> 3(if) -> 4(return)
 *          ^                  |
 *          |                  |
 *          --------------------
 *
 * Complexity of: 5(edges) - 4(nodes) + 2 = 3
 * Test cases:
 *	- No data in list, no next index
 *	- data in list, next index found
 *	- at end of list, no more indices to search
 */
static CBool CIIterator_HasNext_Def( struct CIIterator* self_ )
{
	CAssertObject(self_);
	struct CCListIterator* self = CCast(self_);

	size_t i;
	size_t list_size;
	struct CIList* list;

	list = self->list;
	list_size = CIList_MaxSize(list);

	/* Go through entire list from last known index to find
	 * a location where there is still data.
	 */
	for( i = self->index; i < list_size; ++i ) {
		/* Check if this index, i, has an elemet in it.
		 */
		if( CIList_Get(list, NULL, i) == CILIST_OK ) {
			/* There is an element at index i, so
			 * return logical true.
			 */
			self->next_index = i;
			self->is_next_valid = 1;
			return 1;
		}
	}

	/* There is no next element.
	 */
	self->is_next_valid = 0;
	return 0;
}

/* Complexity map:
 * 1(CCast) -> 2(if is_valid) -> 3(get next) -> 4(return) <-
 *                            |       ^-----------         |
 *                            |                  |         |
 *                            -> 5(if has next) ------------
 *
 * complexity of: 6(edge) - 5(nodes) + 2 = 3
 *
 * test cases:
 *	* next is valid
 *	* next is not valid, but has next
 *	* next is not valid, no has next                                              
 */
static void CIIterator_Next_Def( struct CIIterator* self_, void* element )
{
	CAssertObject(self_);
	struct CCListIterator* self = CCast(self_);

	struct CIList* list = self->list;

	/* Check if a valid index into the next element is cached.
	 * or, if not, check if there is a next element (which will cache
	 * an index).
	 */
	if( self->is_next_valid || CIIterator_HasNext(&self->ciiterator) ) {
		/* Have a valid index into the next element, get that.
		 */
		CIList_Get(list, element, self->next_index);
		self->index = self->next_index + 1;
		self->last_index_used = self->next_index;
		self->is_last_index_used_valid = 1;
	}

	/* Cached index is no longer valid (if it was before),
	 * so, invalidate it. 
	 */
	self->is_next_valid = 0;
}

/* complexity map:
 *                    ----------
 *                    |        |
 * 1(CCast) -> 2(for) -> 3(if) -> 4(return)
 *          ^                  |
 *          |                  |
 *          --------------------
 *
 * Complexity of: 5(edges) - 4(nodes) + 2 = 3
 * Test cases:
 *	- At beginning of list.
 *	- Not at beginning, there is a previous element.
 *	- At end of list, list is now empty.
 */
static CBool CIIterator_HasPrevious_Def( struct CIIterator* self_ )
{
	CAssertObject(self_);
	struct CCListIterator* self = CCast(self_);
	
	size_t i;
	struct CIList* list;

	list = self->list;

	/* Go through entire list from last known index to find
	 * a location where there is still data.
	 */
	for( i = self->index; i > 0; --i ) {
		/* Check if this index, i, has an elemet in it.
		 */
		if( CIList_Get(list, NULL, i-1) == CILIST_OK ) {
			/* There is an element at index i, so
			 * return logical true.
			 */
			self->previous_index = i-1;
			self->is_previous_valid = 1;		      
			return 1;
		}
	}

	/* There is no next element.
	 */
	self->is_next_valid = 0;
	return 0;
}

/* Complexity map:
 * 1(CCast) -> 2(if is_valid) -> 3(get prev) -> 4(return) <-
 *                            |       ^-----------         |
 *                            |                  |         |
 *                            -> 5(if has prev) ------------
 *
 * complexity of: 6(edge) - 5(nodes) + 2 = 3
 *
 * test cases:
 *	* previous is valid
 *	* previous is not valid, but has previous
 *	* previous is not valid, no has previous                                         
 */
static void CIIterator_Previous_Def( struct CIIterator* self_, void* element )
{
	CAssertObject(self_);
	struct CCListIterator* self = CCast(self_);

	struct CIList* list = self->list;

	/* Check if a valid index into the previous element is cached.
	 * or, if not, check if there is a previous element (which will cache
	 * an index).
	 */
	if( self->is_previous_valid || CIIterator_HasPrevious(&self->ciiterator) ) {
		/* Have a valid index into the next element, get that.
		 */
		CIList_Get(list, element, self->previous_index);
		self->index = self->previous_index;
		self->last_index_used = self->previous_index;
		self->is_last_index_used_valid = 1;
	}

	/* Cached index is no longer valid (if it was before),
	 * so, invalidate it. 
	 */
	self->is_previous_valid = 0;
}

static CBool CIIterator_Set_Def( struct CIIterator* self_, void* element )
{
	CAssertObject(self_);
	struct CCListIterator* self = CCast(self_);

	if( self->is_last_index_used_valid ) {
		CIList_AddAt(self->list, element, self->last_index_used);
		return 1;
	}
	
	return 0;
}

static CBool CIIterator_Remove_Def( struct CIIterator* self_ )
{
	CAssertObject(self_);
	struct CCListIterator* self = CCast(self_);

	if( self->is_last_index_used_valid ) {
		CIList_Remove(self->list, NULL, self->last_index_used);
		return 1;
	}
	
	return 0;
}

static size_t CIIterator_Index_Def( struct CIIterator* self_ )
{
	CAssertObject(self_);
	struct CCListIterator* self = CCast(self_);

	if( self->is_last_index_used_valid ) {
		return self->last_index_used;
	}
	
	return 0;
}

/************************************************************************/
/* vtable key								*/
/************************************************************************/
const struct CCListIterator_VTable* CCListIterator_GetVTable( )
{
	static struct CCListIterator_VTable vtable  =
		{
			/* Assign implemenation of interface CIIterator's methods. */
			.ciiterator_override.hasNext = CIIterator_HasNext_Def,
			.ciiterator_override.next = CIIterator_Next_Def,
			.ciiterator_override.hasPrevious = CIIterator_HasPrevious_Def,
			.ciiterator_override.previous = CIIterator_Previous_Def,
			.ciiterator_override.set = CIIterator_Set_Def,
			.ciiterator_override.remove = CIIterator_Remove_Def,
			.ciiterator_override.index = CIIterator_Index_Def
		};

	/* Super's vtable copy. */
	vtable.cobject = *CObject_GetVTable( );

	/* Return pointer to CCArrayList's vtable. */
	return &vtable;
}


/************************************************************************/
/* Constructor								*/
/************************************************************************/
CError CCListIterator( struct CCListIterator* self, struct CIList* list )
{
	CAssertObject(self);
	CAssertObject(list);

	/* First thing in constructor must be to call super's constructor. 
	 */
	CObject(&self->cobject);

	/* Second thing in constructor must be to map vtable. 
	 */
	CVTable(self, CCListIterator_GetVTable( ));

	/* Third thing in constructor must be calling interface's constructor. 
	 */
	CInterface(self, &self->ciiterator, &CCListIterator_GetVTable( )->ciiterator_override);

	/* Set up member data.
	 */
	self->list = list;
	self->index = 0;
	self->next_index = 0;
	self->previous_index = 0;
	self->last_index_used = 0;
	self->is_next_valid = 0;
	self->is_previous_valid = 0;
	self->is_last_index_used_valid = 0;

	return COBJ_OK;
}


	
