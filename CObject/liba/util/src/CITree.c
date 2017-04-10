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

#include <CITree.h>

/************************************************************************/
/* Virtual method wrappers						*/
/************************************************************************/
CITreeError CITree_Push( struct CITree* self, const void* element, const void* key )
{
	CAssertObject(self);
	const struct CITree_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->push);
	return vtable->push(self, element, key);
}

CITreeError CITree_Pop( struct CITree* self, void* element )
{
	CAssertObject(self);
	const struct CITree_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->pop);
	return vtable->pop(self, element);
}

CITreeError CITree_Peek( struct CITree* self, void* element )
{
	CAssertObject(self);
	const struct CITree_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->peek);
	return vtable->peek(self, element);
}

CITreeError CITree_Get( struct CITree* self, void* element, size_t index )
{
	CAssertObject(self);
	const struct CITree_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->get);
	return vtable->get(self, element, index);
}

CITreeError CITree_Delete( struct CITree* self, void* element, size_t index )
{
	CAssertObject(self);
	const struct CITree_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->delete);
	return vtable->delete(self, element, index);
}

CITreeError CITree_DeleteElement( struct CITree* self, void* element )
{
	CAssertObject(self);
	const struct CITree_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->delete_element);
	return vtable->delete_element(self, element);
}

size_t CITree_Size( struct CITree* self )
{
	CAssertObject(self);
	const struct CITree_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->size);
	return vtable->size(self);
}

size_t CITree_MaxSize( struct CITree* self )
{
	CAssertObject(self);
	const struct CITree_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->max_size);
	return vtable->max_size(self);
}

void CITree_Clear( struct CITree* self )
{
	CAssertObject(self);
	const struct CITree_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->clear);
	vtable->clear(self);
}
