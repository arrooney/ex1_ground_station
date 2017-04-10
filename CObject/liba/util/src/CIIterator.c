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
 */

#include <CIIterator.h>

/************************************************************************/
/* Virtual method wrapper defines.					*/
/************************************************************************/
CBool CIIterator_HasNext( struct CIIterator* self )
{
	CAssertObject(self);
	const struct CIIterator_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->hasNext);
	return vtable->hasNext(self);
}

void CIIterator_Next( struct CIIterator* self, void* element )
{
	CAssertObject(self);

	const struct CIIterator_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->next);
	vtable->next(self, element);
}

CBool CIIterator_HasPrevious( struct CIIterator* self )
{
	CAssertObject(self);

	const struct CIIterator_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->hasPrevious);
	return vtable->hasPrevious(self);
}

void CIIterator_Previous( struct CIIterator* self, void* element )
{
	CAssertObject(self);

	const struct CIIterator_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->previous);
	vtable->previous(self, element);
}

CBool CIIterator_Set( struct CIIterator* self, void* element )
{
	CAssertObject(self);

	const struct CIIterator_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->set);
	return vtable->set(self, element);
}

CBool CIIterator_Remove( struct CIIterator* self )
{
	CAssertObject(self);

	const struct CIIterator_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->remove);
	return vtable->remove(self);
}

size_t CIIterator_Index( struct CIIterator* self )
{
	CAssertObject(self);

	const struct CIIterator_VTable* vtable = CGetVTable(self);
	CAssertVirtual(vtable);
	CAssertVirtual(vtable->index);
	return vtable->index(self);
}

