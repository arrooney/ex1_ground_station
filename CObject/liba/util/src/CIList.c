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
#include <CIList.h>

/************************************************************************/
/* Virtual method call wrappers.					*/
/************************************************************************/
CIListError CIList_Add( struct CIList* self, void* element )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIList_VTable*) CGetVTable(self))->add);
	return ((struct CIList_VTable*) CGetVTable(self))->add(self, element);
}

CIListError CIList_AddAt( struct CIList* self, void* element, size_t index )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIList_VTable*) CGetVTable(self))->addAt);
	return ((struct CIList_VTable*) CGetVTable(self))->addAt(self, element, index);
}

CIListError CIList_Get( struct CIList* self, void* element, size_t index )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIList_VTable*) CGetVTable(self))->get);
	return ((struct CIList_VTable*) CGetVTable(self))->get(self, element, index);
}

CIListError CIList_Remove( struct CIList* self, void* element, size_t index )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIList_VTable*) CGetVTable(self))->remove);
	return ((struct CIList_VTable*) CGetVTable(self))->remove(self, element, index);
}

void CIList_Clear( struct CIList* self)
{
	CAssertObject(self);
	CAssertVirtual(((struct CIList_VTable*) CGetVTable(self))->clear);
	((struct CIList_VTable*) CGetVTable(self))->clear(self);
}

size_t CIList_Size( struct CIList* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIList_VTable*) CGetVTable(self))->size);
	return ((struct CIList_VTable*) CGetVTable(self))->size(self);
}

size_t CIList_MaxSize( struct CIList* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIList_VTable*) CGetVTable(self))->maxSize);
	return ((struct CIList_VTable*) CGetVTable(self))->maxSize(self);
}
