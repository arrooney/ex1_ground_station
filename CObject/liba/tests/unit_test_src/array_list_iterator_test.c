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

#include <unit.h>
#include <CCArrayList.h>
#include <CCListIterator.h>

#define DEFAULT_LENGTH 12
#define UNITY_LENGTH 1
#define DEFAULT_ELEMENT_SIZE sizeof(int)

#define CONSTRUCT_ITERATOR( )\
	do {\
		if( CCListIterator(&iterator, &list.cilist) != COBJ_OK ) {\
			ABORT_TEST("Failed to construct iterator");\
		}\
	} while( 0 )


struct CCArrayList list;
struct CCListIterator iterator;

TEST_SETUP( )
{
	CCArrayList(&list, DEFAULT_ELEMENT_SIZE, DEFAULT_LENGTH);
}
TEST_TEARDOWN( )
{
	CDestroy(&list);
}

TEST(has_next)
{
	CBool status;
	int test_var;
	
	/* With no data in the list, the iterator should have no
	 * next element.
	 */
	CONSTRUCT_ITERATOR( );
	status = CIIterator_HasNext(&iterator.ciiterator);
	ASSERT(!status, "Has next returned true on an empty list");
	CDestroy(&iterator);

	/* Add data to the list, the has next method should 
	 * return true.
	 */
	test_var = 0;
	CIList_AddAt(&list.cilist, &test_var, DEFAULT_LENGTH-1);

	/* Now, has next should return true.
	 */
	CONSTRUCT_ITERATOR( );
	status = CIIterator_HasNext(&iterator.ciiterator);
	ASSERT(status, "Has next returned false when there are more elements in the list");

	/* Skip this element using the iterator.
	 */
	CIIterator_Next(&iterator.ciiterator, NULL);

	/* The iterator should now be
	 * at the end of the list. The has next method should now
	 * return false because the internal cache points to the
	 * end of the list.
	 */
	status = CIIterator_HasNext(&iterator.ciiterator);
	ASSERT(!status, "Has next returned true when at the end of the list");
	CDestroy(&iterator);
}

TEST(next)
{      
	int test_var = 0;
	
	/* With no data in the list, the iterator should have no
	 * next element, and hence the next method should fail.
	 */
	CONSTRUCT_ITERATOR( );
	CIIterator_Next(&iterator.ciiterator, &test_var);
	ASSERT(test_var == 0, "next should have done nothing");
	CDestroy(&iterator);

	/* Now, lets add a couple elements and verify the next method
	 * works under two different cases.
	 */
	++test_var;
	CIList_Add(&list.cilist, &test_var);
	++test_var;
	CIList_Add(&list.cilist, &test_var);

	/* First, we will call next with no call to has next, then
	 * we will call has next before calling next.
	 * In both cases, the next method should work.
	 */
	CONSTRUCT_ITERATOR( );
	CIIterator_Next(&iterator.ciiterator, &test_var);
	ASSERT(test_var == 1, "Next did not work without calling has next");
	CIIterator_HasNext(&iterator.ciiterator);
	CIIterator_Next(&iterator.ciiterator, &test_var);
	ASSERT(test_var == 2, "Next did not work after calling has next");
	CDestroy(&iterator);
}

TEST(has_previous)
{
	CBool status;
	int test_var;
	
	/* At beginning of list, there should be no previous element.
	 */
	CONSTRUCT_ITERATOR( );
	status = CIIterator_HasPrevious(&iterator.ciiterator);
	ASSERT(!status, "Has previous returned true when at beginning of list");
	CDestroy(&iterator);

	/* Add data to the list, the has previous method should 
	 * return true.
	 */
	test_var = 0;
	CIList_AddAt(&list.cilist, &test_var, DEFAULT_LENGTH-1);

	/* Now, has previous should return true.
	 */
	CONSTRUCT_ITERATOR( );
	CIIterator_Next(&iterator.ciiterator, NULL);
	status = CIIterator_HasPrevious(&iterator.ciiterator);
	ASSERT(status, "Has previous returned false when there are more elements in the list");

	/* Remove this element using the iterator.
	 */
	CIIterator_Remove(&iterator.ciiterator);

	/* The iterator should now be now be empty with the internal
	 * cache at the end of the list. The has previous method should
	 * reverse search the entire list, find nothing, and return false.
	 */
	status = CIIterator_HasPrevious(&iterator.ciiterator);
	ASSERT(!status, "Has previous returned true when the list is empty");
	CDestroy(&iterator);
}

TEST(previous)
{      
	int test_var = 0;
	
	/* With no data in the list, the iterator should have no
	 * previous element, and hence the previous  method should fail.
	 */
	CONSTRUCT_ITERATOR( );
	CIIterator_Previous(&iterator.ciiterator, &test_var);
	ASSERT(test_var == 0, "Previous should have done nothing");
	CDestroy(&iterator);

	/* Now, lets add a couple elements and verify the previous method
	 * works under two different cases.
	 */
	++test_var;
	CIList_Add(&list.cilist, &test_var);
	++test_var;
	CIList_Add(&list.cilist, &test_var);

	/* First, we have to iterate through the list slightly
	 * so that there are previous elements.
	 */
	CONSTRUCT_ITERATOR( );
	CIIterator_Next(&iterator.ciiterator, NULL);
	CIIterator_Next(&iterator.ciiterator, NULL);
	
	/* Now, we will call previous with no call to has previous, then
	 * we will call has previous before calling previous.
	 * In both cases, the previous method should work.
	 */
	CIIterator_Previous(&iterator.ciiterator, &test_var);
	ASSERT(test_var == 2, "Previous did not work without calling has previous");
	CIIterator_HasPrevious(&iterator.ciiterator);
	CIIterator_Previous(&iterator.ciiterator, &test_var);
	ASSERT(test_var == 1, "Previous did not work after calling has previous");
	CDestroy(&iterator);
}

TEST(set)
{
	int test_var = 0;
	CBool status;
	
	/* Add a couple elements and verify the iterator can set there value.
	 */
	CIList_AddAt(&list.cilist, &test_var, 0);
	++test_var;
	CIList_AddAt(&list.cilist, &test_var, 1);
	++test_var;
	CIList_AddAt(&list.cilist, &test_var, 2);
	CONSTRUCT_ITERATOR( );

	/* Have never used next/previous so setting should fail.
	 */
	status = CIIterator_Set(&iterator.ciiterator, &test_var);
	ASSERT(!status, "Set an element when the index was undefined");

	/* Index | value
	 *   0      1
	 *   1      2
	 *   2      3
	 *   2      4
	 *   1      3
	 */
	CIIterator_Next(&iterator.ciiterator, &test_var);
	++test_var;
	status = CIIterator_Set(&iterator.ciiterator, &test_var);
	ASSERT(status, "Failed to set index 0");

	CIIterator_Next(&iterator.ciiterator, &test_var);
	++test_var;
	status = CIIterator_Set(&iterator.ciiterator, &test_var);
	ASSERT(status, "Failed to set index 1");

	CIIterator_Next(&iterator.ciiterator, &test_var);
	++test_var;
	status = CIIterator_Set(&iterator.ciiterator, &test_var);
	ASSERT(status, "Failed to set index 2");

	CIIterator_Previous(&iterator.ciiterator, &test_var);
	++test_var;
	status = CIIterator_Set(&iterator.ciiterator, &test_var);
	ASSERT(status, "Failed to set index 2");

	CIIterator_Previous(&iterator.ciiterator, &test_var);
	++test_var;
	status = CIIterator_Set(&iterator.ciiterator, &test_var);
	ASSERT(status, "Failed to set index 2");

	CDestroy(&iterator);

	/* Verify the value were set.
	 */
	CIList_Get(&list.cilist, &test_var, 0);
	ASSERT(test_var == 1, "Index 0 didn't get set, it is %d", test_var);

	CIList_Get(&list.cilist, &test_var, 1);
	ASSERT(test_var == 3, "Index 1 didn't get set, it is %d", test_var);

	CIList_Get(&list.cilist, &test_var, 2);
	ASSERT(test_var == 4, "Index 2 didn't get set, it is %d", test_var);
}

TEST(remove)
{
	int test_var = 0;
	CBool status;
	CIListError err;
	
	/* Add a couple elements and verify the iterator can set there value.
	 */
	CIList_AddAt(&list.cilist, &test_var, 0);
	++test_var;
	CIList_AddAt(&list.cilist, &test_var, 1);
	++test_var;
	CIList_AddAt(&list.cilist, &test_var, 2);
	CONSTRUCT_ITERATOR( );

	/* Have never used next/previous so removing should fail.
	 */
	status = CIIterator_Remove(&iterator.ciiterator);
	ASSERT(!status, "Removed an element when the index was undefined");

	/* Remove at index 0.
	 */
	CIIterator_Next(&iterator.ciiterator, &test_var);
	status = CIIterator_Remove(&iterator.ciiterator);
	ASSERT(status, "Failed to remove index 0");

	/* Remove at index 1.
	 */
	CIIterator_Next(&iterator.ciiterator, &test_var);
	status = CIIterator_Remove(&iterator.ciiterator);
	ASSERT(status, "Failed to remove index 1");

	/* Go to index 2.
	 */
	CIIterator_Next(&iterator.ciiterator, &test_var);

	/* Goes backwards to index 2 and remove at index 2.
	 */
	CIIterator_Previous(&iterator.ciiterator, &test_var);
	status = CIIterator_Remove(&iterator.ciiterator);
	ASSERT(status, "Failed to remove index 2");

	CDestroy(&iterator);

	/* Verify the value were removed
	 */
	err = CIList_Get(&list.cilist, &test_var, 0);
	ASSERT(err == CILIST_ERR_EMPTY, "Index 0 didn't get removed");

	err = CIList_Get(&list.cilist, &test_var, 1);
	ASSERT(err == CILIST_ERR_EMPTY, "Index 1 didn't get removed");

	err = CIList_Get(&list.cilist, &test_var, 2);
	ASSERT(err == CILIST_ERR_EMPTY, "Index 2 didn't get removed");
}
     
TEST(index)
{
	size_t index;
	int test_var = 0;
	
	/* Add a couple elements and verify the iterator can get the 
	 * index of them.
	 */
	++test_var;
	CIList_AddAt(&list.cilist, &test_var, 0);
	++test_var;
	CIList_AddAt(&list.cilist, &test_var, 1);
	++test_var;
	CIList_AddAt(&list.cilist, &test_var, 2);
	CONSTRUCT_ITERATOR( );

	/* Have never used next/previous so the get index should
	 * fail. It does this by returning zero.
	 */
	index = CIIterator_Index(&iterator.ciiterator);
	ASSERT(index == 0, "Got a non zero index on clean iterator");

	/* Index should return zero, then one, then two, then two, then one.
	 */
	CIIterator_Next(&iterator.ciiterator, &test_var);
	index = CIIterator_Index(&iterator.ciiterator);
	ASSERT(index == 0, "Index of element zero returned index %zu", index);

	CIIterator_Next(&iterator.ciiterator, &test_var);
	index = CIIterator_Index(&iterator.ciiterator);
	ASSERT(index == 1, "Index of element one returned index %zu", index);

	CIIterator_Next(&iterator.ciiterator, &test_var);
	index = CIIterator_Index(&iterator.ciiterator);
	ASSERT(index == 2, "Index of element two returned index %zu", index);

	CIIterator_Previous(&iterator.ciiterator, &test_var);
	index = CIIterator_Index(&iterator.ciiterator);
	ASSERT(index == 2, "Index of element two from previous returned index %zu", index);

	CIIterator_Previous(&iterator.ciiterator, &test_var);
	index = CIIterator_Index(&iterator.ciiterator);
	ASSERT(index == 1, "Index of element one from previous returned index %zu", index);
	
	CDestroy(&iterator);	
}

TEST_SUITE(array_list_iterator)
{
	ADD_TEST(has_next);
	ADD_TEST(next);
	ADD_TEST(has_previous);
	ADD_TEST(previous);
	ADD_TEST(set);
	ADD_TEST(remove);
	ADD_TEST(index);
}
