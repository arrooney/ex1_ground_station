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

#define DEFAULT_LENGTH 12
#define UNITY_LENGTH 1
#define DEFAULT_ELEMENT_SIZE sizeof(int)

struct CCArrayList* list;
struct CCArrayList static_list;
struct CCArrayList dynamic_list;
char static_list_memory[CCARRAY_LIST_SIZE(DEFAULT_ELEMENT_SIZE, DEFAULT_LENGTH)];

TEST_SETUP( )
{
	CCArrayList(&dynamic_list, DEFAULT_ELEMENT_SIZE, DEFAULT_LENGTH);
	CCArrayListStatic(&static_list, DEFAULT_ELEMENT_SIZE, DEFAULT_LENGTH, static_list_memory);
}
TEST_TEARDOWN( )
{
	CDestroy(&dynamic_list);
	CDestroy(&static_list);
}


TEST(add_at)
{
	CIListError 	err;
	int		test_var = 3;

	/* Insert with an out of bounds index.
	 */
	err = CIList_AddAt(&list->cilist, &test_var, DEFAULT_LENGTH+1);
	ASSERT(err == CILIST_ERR_INDEX, "Failure to catch out of bounds index");

	/* Inserting within bounds should return OK. 
	 */
	err = CIList_AddAt(&list->cilist, &test_var, 0);
	ASSERT(err == CILIST_OK, "Failure to do clean insert");
}

TEST(get)
{
	CIListError 	err;
	int 		test_var = 0;

	/* Try to get element with out of bounds index.
	 */
	err = CIList_Get(&list->cilist, &test_var, DEFAULT_LENGTH+1);
	ASSERT(err == CILIST_ERR_INDEX, "Failure to catch out of bounds index");

	/* Try to get element at empty location. All locations are
	 * empty at this point, so can use any index in bounds.
	 */
	err = CIList_Get(&list->cilist, &test_var, 0);
	ASSERT(err == CILIST_ERR_EMPTY, "Failure to catch empty index");

	/* Insert an element into the list to assert the get method
	 * works with in bounds, non empty indices.
	 */
	CIList_AddAt(&list->cilist, &test_var, 0);

	/* Assert we can get this element without error now.
	 */
	test_var = !test_var;
	err = CIList_Get(&list->cilist, &test_var, 0);
	ASSERT(err == CILIST_OK, "Failed to get element at clean index");
	ASSERT(test_var == 0, "Failed to get correct element at clean index");

	/* Assert we can get the same element, but ignoring its value.
	 */
	err = CIList_Get(&list->cilist, NULL, 0);
	ASSERT(err == CILIST_OK, "Failed to get element at clean index");
}

TEST(remove)
{
	CIListError	err;
	int		test_var = 0;

	/* Test this method fails with an error on the same conditions
	 * that CIList_Get( ) would.
	 */
	err = CIList_Remove(&list->cilist, &test_var, DEFAULT_LENGTH+1);
	ASSERT(err == CILIST_ERR_INDEX, "Failed to throw same error as CIList_Get");

	/* Insert an element into the list to assert the remove method
	 * works with clean parameters.
	 */
	CIList_AddAt(&list->cilist, &test_var, 0);

	/* Assert we can get this element without error now.
	 */
	test_var = !test_var;
	err = CIList_Remove(&list->cilist, &test_var, 0);
	ASSERT(err == CILIST_OK, "Failed to remove element at clean index");
	ASSERT(test_var == 0, "Failed to remove correct element at clean index");

	/* Removing at this index should now return an empty error.
	 */
	err = CIList_Remove(&list->cilist, &test_var, 0);
	ASSERT(err == CILIST_ERR_EMPTY, "Failed to throw error on empty index");
}

TEST(add)
{
	CIListError	err;
	int		test_var = 0;
	int i;

	/* Assert no error when adding at an empty index.
	 */
	err = CIList_Add(&list->cilist, &test_var);
	ASSERT(err == CILIST_OK, "Failed to insert with clean parameters");

	/* Assert a new index can be found, do this by adding something
	 * to index one, which is the index CIList_Add will use for
	 * its next insert.
	 */
	++test_var;
	CIList_AddAt(&list->cilist, &test_var, 1);
	++test_var;
	err = CIList_Add(&list->cilist, &test_var);
	ASSERT(err == CILIST_OK, "Failed to find new index with clean parameters");

	/* Fill the entire list, this way we can test a new index will
	 * not be found.
	 */
	for( i = 3; i < DEFAULT_LENGTH; ++i ) {
		++test_var;
		CIList_AddAt(&list->cilist, &test_var, i);
	}

	/* Now test a new index can't be found.
	 */
	++test_var;
	err = CIList_Add(&list->cilist, &test_var);
	ASSERT(err == CILIST_ERR_FULL, "Failed to throw a full list error");
}

TEST(clear)
{
	CIListError 	err;
	int		test_var, i;

	/* Fill the entire list.
	 */
	for( i = 0; i < DEFAULT_LENGTH; ++i ) {
		err = CIList_Add(&list->cilist, &i);
	}

	/* Clear the list.
	 */
	CIList_Clear(&list->cilist);

	/* Fill the entire list up again.
	 */
	for( i = 0; i < DEFAULT_LENGTH; ++i ) {
		err = CIList_Add(&list->cilist, &i);

		/* Assert there was no error adding the element,
		 * and assert the current size increased.
		 */
		ASSERT(err == CILIST_OK, "Failed to insert at iteration %d", i);
		ASSERT(CIList_Size(&list->cilist) == (size_t) (i+1), "Failed to track list size");
	}

	/* Assert each element can be removed correctly.
	 */
	for( i = 0; i < DEFAULT_LENGTH; ++i ) {
		err = CIList_Remove(&list->cilist, &test_var, i);

		/* Assert no error and correct element was got.
		 */
		ASSERT(err == CILIST_OK, "Failed to remove at iteration %d", i);
		ASSERT(test_var == i, "Failed to remove correctly at iteration %d", i);
		ASSERT(CIList_Size(&list->cilist) == (size_t) DEFAULT_LENGTH - i - 1,
		       "Size did not decrease at iteration %d", i);
	}
	
	
}

TEST(normal_operation)
{
	CIListError 	err;
	int		test_var, i;

	/* Fill the entire list.
	 */
	for( i = 0; i < DEFAULT_LENGTH; ++i ) {
		err = CIList_Add(&list->cilist, &i);

		/* Assert there was no error adding the element,
		 * and assert the current size increased.
		 */
		ASSERT(err == CILIST_OK, "Failed to insert at iteration %d", i);
		ASSERT(CIList_Size(&list->cilist) == (size_t) (i+1), "Failed to track list size");
	}

	/* Assert each element was correctly added.
	 */
	for( i = 0; i < DEFAULT_LENGTH; ++i ) {
		err = CIList_Get(&list->cilist, &test_var, i);

		/* Assert no error and correct element was got.
		 */
		ASSERT(err == CILIST_OK, "Failed to get at iteration %d", i);
		ASSERT(test_var == i, "Failed to get correctly at iteration %d", i);
		ASSERT(CIList_Size(&list->cilist) == DEFAULT_LENGTH, "Size did not stay constant at iteration %d", i);
	}

	/* Assert each element can be removed.
	 */
	for( i = 0; i < DEFAULT_LENGTH; ++i ) {
		err = CIList_Remove(&list->cilist, &test_var, i);

		/* Assert no error and correct element was got.
		 */
		ASSERT(err == CILIST_OK, "Failed to remove at iteration %d", i);
		ASSERT(test_var == i, "Failed to remove correctly at iteration %d", i);
		ASSERT(CIList_Size(&list->cilist) == (size_t) DEFAULT_LENGTH - i - 1, "Size did not decrease at iteration %d", i);
	}

	/* Assert each element was correctly removed and we cannot
	 * try and get it again.
	 */
	for( i = 0; i < DEFAULT_LENGTH; ++i ) {
		err = CIList_Get(&list->cilist, &test_var, i);

		/* Assert no error and correct element was got.
		 */
		ASSERT(err == CILIST_ERR_EMPTY, "Failed to mark as removed at iteration %d", i);
	}

	/* Fill the list back up so we can assert its state doesn't get corrupt
	 * during normal add and remove operation.
	 */
	for( i = 0; i < DEFAULT_LENGTH; ++i ) {
		err = CIList_Add(&list->cilist, &i);

		/* Assert there was no error adding the element,
		 * and assert the current size increased.
		 */
		ASSERT(err == CILIST_OK, "Failed to re-insert at iteration %d", i);
	}

	/* Assert each element can be removed as before.
	 */
	for( i = 0; i < DEFAULT_LENGTH; ++i ) {
		err = CIList_Remove(&list->cilist, &test_var, i);

		/* Assert no error and correct element was got.
		 */
		ASSERT(err == CILIST_OK, "Failed to remove at during second iteration %d", i);
		ASSERT(test_var == i, "Failed to remove correctly at second iteration %d", i);
		ASSERT(CIList_Size(&list->cilist) == (size_t) DEFAULT_LENGTH - i - 1,
		       "Size did not decrease at second iteration %d", i);
	}
	
}

TEST(max_size)
{
	ASSERT(CIList_MaxSize(&list->cilist) == DEFAULT_LENGTH, "Failed to get max size");
}

TEST(depth_one)
{
	/* Test a list of size one works correctly.
	 */
	struct CCArrayList oneList;
	CIListError err;
	int test_var;

	CCArrayList(&oneList, sizeof(test_var), 1);

	test_var = 0;
	err = CIList_Add(&oneList.cilist, &test_var);
	ASSERT(err == CILIST_OK, "Failed to add");

	++test_var;
	err = CIList_Add(&oneList.cilist, &test_var);
	ASSERT(err == CILIST_ERR_FULL, "Added to full list by error");

	err = CIList_Get(&oneList.cilist, &test_var, 0);
	ASSERT(err == CILIST_OK, "Failed to get from list");
	ASSERT(test_var == 0, "Failed to get correct value from list");

	err = CIList_Get(&oneList.cilist, &test_var, 0);
	ASSERT(err == CILIST_OK, "Failed to remove from list");
	ASSERT(test_var == 0, "Failed to remove correct value from list");

	CDestroy(&oneList);
}

TEST_SUITE(array_list)
{
	/* Run tests using dynamic list. 
	 */
	UNIT_PRINT("Running dynamic list tests...\n");
	list = &dynamic_list;
	ADD_TEST(add_at);
	ADD_TEST(get);
	ADD_TEST(remove);
	ADD_TEST(add);
	ADD_TEST(clear);
	ADD_TEST(max_size);
	ADD_TEST(normal_operation);

	/* Run tests using static list. 
	 */
	UNIT_PRINT("Running static list tests...\n");
	list = &static_list;
	ADD_TEST(add_at);
	ADD_TEST(get);
	ADD_TEST(remove);
	ADD_TEST(add);
	ADD_TEST(clear);
	ADD_TEST(max_size);
	ADD_TEST(normal_operation);

	
	ADD_TEST(depth_one);
}

