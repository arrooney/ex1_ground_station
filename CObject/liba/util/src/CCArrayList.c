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

#include <CCArrayList.h>
#include <string.h>

/* These must keep their respective 0 and 1 values
 * for the bit mask to work correctly. 
 */
#define CCARRAY_LIST_MASK_ELEMENT_EMPTY	0
#define CCARRAY_LIST_MASK_ELEMENT_FULL	1

/* There is one bit for every element in the list.
 * To get the number of bytes required to for a one to one
 * map of element index's to bits divide the max size by the
 * number of bits in a byte, then add one to ensure
 * a round up. 
 */
#define CCARRAY_LIST_MASK_SIZE( list_size ) (((list_size) >> 3) + 1)

/* Get the byte offset into the list mask given
 * the index of an element in the list.
 */
#define CCARRAY_LIST_MASK_INDEX( element_index )\
	((element_index) >> 3)

/* Get the bit offset into a mask byte given
 * the index into an element in the list.
 */
#define CCARRAY_LIST_MASK_BIT( mask_byte, element_index )\
	((element_index) & 0x07)


/************************************************************************/
/* Private methods							*/
/************************************************************************/
/* Resets the list back to an empty state.
 */
static void CCArrayList_Clear( struct CCArrayList* self )
{
	CAssertObject(self);

	self->_.current_size = 0;
	self->_.add_index = 0;

	/* Reset the list mask to zero. */
	memset(self->_.list_mask, 0, CCARRAY_LIST_MASK_SIZE(self->_.max_size));
}

/* Given an index into the list, this returns the empty/full bit mask
 * of the index. Checking the returned value against 
 * CCARRAY_LIST_MASK_ELEMENT_EMPTY and CCARRAY_LIST_MASK_ELEMENT_FULL
 * will indicate if the index has valid data in it or not.
 * Bounds checking is not done on the index, this must be done before
 * calling the method.
 */
static unsigned char CCArrayList_GetMaskBit( struct CCArrayList* self, size_t index )
{
	size_t mask_index;
	unsigned char mask_bit;
	unsigned char mask_byte;
	CAssertObject(self);    

	/* Get an index into the list mask. 
	 */
	mask_index = CCARRAY_LIST_MASK_INDEX(index);

	/* Get the byte in the mask list containing the bit
	 */
	mask_byte = self->_.list_mask[mask_index];

	/* Get the bit offset within the byte of interest.
	 */
	mask_bit = CCARRAY_LIST_MASK_BIT(mask_byte, index);

	/* Get the value of the bit.
	 */
	return ((mask_byte >> mask_bit) & 0x01);
}

/* Used to set the empty/full status of an index in the list. The status
 * must be set with one of:
 * CCARRAY_LIST_MASK_ELEMENT_EMPTY or CCARRAY_LIST_MASK_ELEMENT_FULL
 * and the index must be bounds checked before calling this method.
 */
static void CCArrayList_SetMaskBit( struct CCArrayList* self, size_t index, unsigned char val )
{
	size_t mask_index;
	unsigned char mask_bit;
	CAssertObject(self);    

	/* Get an index into the list mask. 
	 */
	mask_index = CCARRAY_LIST_MASK_INDEX(index);

	/* Get the bit offset within the byte of interest.
	 */
	mask_bit = CCARRAY_LIST_MASK_BIT(mask_byte, index);

	if( val == 0 ) {
		/* Set the bit to zero.
		 */
		self->_.list_mask[mask_index] &= ((0xFEu << mask_bit) | (0x7Fu >> (7 - mask_bit)));
	}
	else if( val == 1 ) {
		/* Set the bit to one.
		 */
		self->_.list_mask[mask_index] |= (0x01u << mask_bit);
	}
}

/************************************************************************/
/* Virtual methods.							*/
/************************************************************************/
/* Complexity map
 * 1(CCast) -> 2(if) -> 3(add_at) -> 4(return) <---------
 *                   |                               |  |
 *                   -> 5(for) -> 6(if) -> 7(add_at) -  |
 *                   |                  |               |
 *                   --------------------               |
 *                                      |               |
 *                                  (for end)           |
 *                                      |               |
 *                                      -----------------
 * This gives a complexity of 9(edges) - 7(nodes) + 2 = 4
 *
 * Test cases should be:
 *	* add index is empty
 *	* add index is full, a new add index can be found.
 *	* add index if full, a new add index cannot be found.
 */
static CIListError CIList_Add_Def( struct CIList* self_, void* element )
{	
	CAssertObject(self_);
	struct CCArrayList* self = CCast(self_);

	/* Check if the add index is empty.
	 */
	if( CCArrayList_GetMaskBit(self, self->_.add_index) == CCARRAY_LIST_MASK_ELEMENT_EMPTY ) {
		/* There is no valid data at the add index, so we can
		 * increment it and insert the element.
		 */
		CIListError err;
		err = CIList_AddAt(&self->cilist, element, self->_.add_index);
		self->_.add_index = (self->_.add_index + 1) % self->_.max_size;
		return err;
	}

	/* Need to find a location to add the element, since the current add index 
	 * has valid data that we don't want to overwrite.
	 */
	size_t i;
	for( i = 0; i < self->_.max_size; ++i ) {
		if( CCArrayList_GetMaskBit(self, i) == CCARRAY_LIST_MASK_ELEMENT_EMPTY ) {
			/* Found an empty index.
			 * Increment add index and insert element.
			 */
			CIListError err;
			self->_.add_index = i;
			err = CIList_AddAt(&self->cilist, element, self->_.add_index);
			self->_.add_index = (self->_.add_index + 1) % self->_.max_size;
			return err;
		}
	}

	/* The list is full, no empty spaces to insert.
	 */
	return CILIST_ERR_FULL;
}

/* Complexity map
 * 1(CCast) -> 2(if) -> 3(return) <-------------
 *                   |                         |
 *                   -> 4(copy) -> 5(set_mask) -
 *
 * This gives complexity of 5(edges) - 5(nodes) + 2 = 2
 *
 * Test cases should be:
 *	* Index out of bounds.
 *	* Index not out of bounds.
 */
static CIListError CIList_AddAt_Def( struct CIList* self_, void* element, size_t index )
{
	CAssertObject(self_);
	struct CCArrayList* self = CCast(self_);

	/* Bounds check the index.
	 */
	if( index >= self->_.max_size ) {
		return CILIST_ERR_INDEX;
	}

	/* Copy data into the list.
	 */
	memcpy(&self->_.list_base[index * self->_.element_size], element, self->_.element_size);

	/* Set the list mask to indicate this location is not empty.
	 */
	CCArrayList_SetMaskBit(self, index, CCARRAY_LIST_MASK_ELEMENT_FULL);

	/* Increase current size.
	 */
	++self->_.current_size;
	
	return CILIST_OK;
}

/* Complexity map:
 * 1(CCast) -> 2(if index >= max size) -> 3(return) <----------
 *                                     |                   |  |
 *                                     -> 4(if mask empty) -  |
 *                                                         |  |
 *                        - 6(copy) <- 5(if element NULL) <-  |
 *                        |          |                        |
 *                        -------------------------------------
 *
 * This gives complexity of 8(edges) - 6(nodes) + 2 = 4
 *
 * Test cases should be:
 *	* Index out of bounds
 *	* Index in bounds, mask is empty
 *	* Index in bounds, mask full, element NULL
 *	* Index in bounds, mask full, element non NULL 
 */                                                         
static CIListError CIList_Get_Def( struct CIList* self_, void* element, size_t index )
{
	CAssertObject(self_);
	struct CCArrayList* self = CCast(self_);

	/* Bounds check the index.
	 */
	if( index >= self->_.max_size ) {
		return CILIST_ERR_INDEX;
	}

	/* Check if there is anythign at this index to get.
	 */
	if( CCArrayList_GetMaskBit(self, index) == CCARRAY_LIST_MASK_ELEMENT_EMPTY ) {
		return CILIST_ERR_EMPTY;
	}
	
	/* Copy data out of the list.
	 */
	if( element != NULL ) {
		memcpy(element, &self->_.list_base[index * self->_.element_size], self->_.element_size);
	}

	return CILIST_OK;
}

/* Complexity map:
 * 1(CCast) -> 2(if err != OK ) -> 3(return) <----
 *                              |                |
 *                              -> 4(clear mask) -
 *
 * This gives complexity of 4(edges) - 4(nodes) + 2 = 2
 *
 * Test cases:
 *	* A condition which makes CIList_Get fail, this could be out of bounds index.
 *	* CIList_Get succeeds
 */ 
static CIListError CIList_Remove_Def( struct CIList* self_, void* element, size_t index )
{
	CAssertObject(self_);
	struct CCArrayList* self = CCast(self_);

	/* Get the element that is going to be removed. 
	 */
	CIListError err = CIList_Get(&self->cilist, element, index);
	if( err != CILIST_OK )  {
		return err;
	}

	/* If there was no error, we got an element out of the list.
	 * Now it needs to marked in the list mask as empty. 
	 */
	CCArrayList_SetMaskBit(self, index, CCARRAY_LIST_MASK_ELEMENT_EMPTY);

	/* Decrease current size.
	 */
	--self->_.current_size;
	
	return CILIST_OK;
}

static void CIList_Clear_Def( struct CIList* self_ )
{
	struct CCArrayList* self = CCast(self_);
	CCArrayList_Clear(self);
}

static size_t CIList_Size_Def( struct CIList* self_ )
{
	struct CCArrayList* self = CCast(self_);
	return self->_.current_size;
}

static size_t CIList_MaxSize_Def( struct CIList* self_ )
{
	struct CCArrayList* self = CCast(self_);
	return self->_.max_size;
}


/************************************************************************/
/* Overriding 								*/
/************************************************************************/
static void CDestructor( void* self_ )
{
	struct CCArrayList* self = CCast(self_);

	if( !self->_.is_static ) {	
		CFree(self->_.list_base);
		CFree(self->_.list_mask);
	}

	/* Call super's destructor
	 */
	const struct CCArrayList_VTable* vtable = CGetVTable(self);
	vtable->cobject->CDestructor(self);
}

/************************************************************************/
/* vtable key								*/
/************************************************************************/
const struct CCArrayList_VTable* CCArrayList_GetVTable( )
{
	static struct CCArrayList_VTable vtable  =
		{
			/* Assign implemenation of interface CIQueue's methods. */
			.cilist_override.add = CIList_Add_Def,
			.cilist_override.addAt = CIList_AddAt_Def,
			.cilist_override.get = CIList_Get_Def,
			.cilist_override.remove = CIList_Remove_Def,
			.cilist_override.clear = CIList_Clear_Def,
			.cilist_override.size = CIList_Size_Def,
			.cilist_override.maxSize = CIList_MaxSize_Def
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
CError CCArrayList( struct CCArrayList* self, size_t element_size, size_t max_size )
{
	/* First thing in constructor must be to call super's constructor. 
	 */
	CObject(&self->cobject);

	/* Second thing in constructor must be to map vtable. 
	 */
	CVTable(self, CCArrayList_GetVTable( ));

	/* Third thing in constructor must be calling interface's constructor. 
	 */
	CInterface(self, &self->cilist, &CCArrayList_GetVTable( )->cilist_override);

	/* Allocate memory for the list. 
	 */
	self->_.list_base = CMalloc(element_size * max_size);
	if( self->_.list_base == NULL ) {
		return COBJ_ALLOC_FAIL;
	}

	/* Allocate memory for the mask. Since we only need one bit
	 * for every element, we divide max size by three, and add one 
	 * to ensure enough bits for every element. 
	 */
	self->_.list_mask = CMalloc(CCARRAY_LIST_MASK_SIZE(max_size));
	if( self->_.list_mask == NULL ) {
		CFree(self->_.list_base);
		return COBJ_ALLOC_FAIL;
	}

	/* All allocations were successful, set up the struct's member variables.
	 */
	self->_.max_size = max_size;
	self->_.current_size = 0;
	self->_.element_size = element_size;
	self->_.add_index = 0;

	/* Clear the list mask to zero. 
	 */
	CCArrayList_Clear(self);

	/* List was allocated within the scope of this structure.
	 */
	self->_.is_static = 0;
	
	return COBJ_OK;
}


CError CCArrayListStatic
(
	struct CCArrayList* self,
	size_t element_size,
	size_t max_size,
	void* memory
)
{
	/* First thing in constructor must be to call super's constructor. 
	 */
	CObject(&self->cobject);

	/* Second thing in constructor must be to map vtable. 
	 */
	CVTable(self, CCArrayList_GetVTable( ));

	/* Third thing in constructor must be calling interface's constructor. 
	 */
	CInterface(self, &self->cilist, &CCArrayList_GetVTable( )->cilist_override);

	/* Assign the base of the list to provided memory block
	 */
	self->_.list_base = memory;

	/* Assign list mask to latter part of provided memory block.
	 */
	self->_.list_mask = ((unsigned char*) memory) + (element_size * max_size);

	/* All allocations were successful, set up the struct's member variables.
	 */
	self->_.max_size = max_size;
	self->_.current_size = 0;
	self->_.element_size = element_size;
	self->_.add_index = 0;

	/* Clear the list mask to zero. 
	 */
	CCArrayList_Clear(self);

	/* List was allocated outside this structure's scope.
	 */
	self->_.is_static = 1;
	
	return COBJ_OK;
}
