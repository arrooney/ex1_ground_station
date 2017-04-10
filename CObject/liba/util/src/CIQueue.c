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

#include <CIQueue.h>

/************************************************************************/
/* Virtual method call wrappers.					*/
/************************************************************************/
CIQueueError CIQueue_Insert( struct CIQueue* self, const void* element )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIQueue_VTable*) CGetVTable(self))->insert);
	return ((struct CIQueue_VTable*) CGetVTable(self))->insert(self, element);
}

CIQueueError CIQueue_Remove( struct CIQueue* self, void* element )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIQueue_VTable*) CGetVTable(self))->remove);
	return ((struct CIQueue_VTable*) CGetVTable(self))->remove(self, element);
}

CIQueueError CIQueue_Peek( struct CIQueue* self, void* element )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIQueue_VTable*) CGetVTable(self))->peek);
	return ((struct CIQueue_VTable*) CGetVTable(self))->peek(self, element);
}

size_t CIQueue_Size( struct CIQueue* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIQueue_VTable*) CGetVTable(self))->size);
	return ((struct CIQueue_VTable*) CGetVTable(self))->size(self);
}

size_t CIQueue_MaxSize( struct CIQueue* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIQueue_VTable*) CGetVTable(self))->maxSize);
	return ((struct CIQueue_VTable*) CGetVTable(self))->maxSize(self);
}

void CIQueue_Clear( struct CIQueue* self )
{
	CAssertObject(self);
	CAssertVirtual(((struct CIQueue_VTable*) CGetVTable(self))->clear);
	((struct CIQueue_VTable*) CGetVTable(self))->clear(self);
}
