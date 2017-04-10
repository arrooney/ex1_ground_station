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

#include <unit.h>
#include <CCBinaryTree.h>

#define TREE_NODE_SIZE sizeof(int)
#define TREE_MAX_SIZE 8
#define TREE_KEY_SIZE sizeof(int)
#define INITIAL_KEY 0

struct CCBinaryTree* tree;
struct CCBinaryTree dynamic_tree;

static signed char compare( const void* key1_, const void* key2_ )
{
	int key1 = *((int*) key1_);
	int key2 = *((int*) key2_);

	return key1 < key2 ? -1 : (key1 > key2 ? 1 : 0);
}

TEST_SETUP( )
{
	CCBinaryTree(&dynamic_tree,
		     TREE_NODE_SIZE,
		     TREE_MAX_SIZE,
		     compare,
		     TREE_KEY_SIZE);
}

TEST_TEARDOWN( )
{
	CDestroy(&dynamic_tree);
}

TEST(push)
{
	int item = 0;
	int key = INITIAL_KEY + 1;
	CITreeError err;

	/* Empty tree, push an item into it.
	 */
	err = CITree_Push(&tree->citree, &item, &key);
	ASSERT(err == CITREE_OK, "Error %d inserting into empty tree", err);

	/* Insert two item's that will be children of the above insert. */
	key += 2;
	++item;
	err = CITree_Push(&tree->citree, &item, &key);
	ASSERT(err == CITREE_OK, "Error %d inserting child 1", err);
	++key;
	++item;
	err = CITree_Push(&tree->citree, &item, &key);
	ASSERT(err == CITREE_OK, "Error %d inserting child 2", err);

	/* Insert a fourth element that gets heapified up only one time.
	 */
	key = INITIAL_KEY + 2;
	++item;
	err = CITree_Push(&tree->citree, &item, &key);
	ASSERT(err == CITREE_OK, "Error %d with heapify before root", err);

	/* Insert a fifth element that gets heapified up to the root.
	 */
	key = INITIAL_KEY;
	++item;
	err = CITree_Push(&tree->citree, &item, &key);
	ASSERT(err == CITREE_OK, "Error %d with heapify up to root", err);

	/* Fill the tree and assert it returns a tree full error.
	 */
	int i;
	for( i = CITree_Size(&tree->citree); i < TREE_MAX_SIZE; ++i ) {
		++item;
		++key;
		err = CITree_Push(&tree->citree, &item, &key);
		ASSERT(err == CITREE_OK, "Error %d inserting during push %d", err, i);
	}
	err = CITree_Push(&tree->citree, &item, &key);
	ASSERT(err == CITREE_ERR_FULL, "No full error on full tree, got %d", err);
}

TEST(get)
{
	int item = 299;
	int key = 0;
	CITreeError err;

	/* Empty tree, get should return an error.
	 */
	err = CITree_Get(&tree->citree, &item, 0);
	ASSERT(err == CITREE_ERR_EMPTY, "No empty error pulling from empty tree, got %d", err);

	/* Put an itme into tree and get it.
	 */
	CITree_Push(&tree->citree, &item, &key);
	item = 0;
	err = CITree_Get(&tree->citree, &item, 0);
	ASSERT(err == CITREE_OK, "Error %d getting from tree", err);
	ASSERT(item == 299, "Got wrong item, %d, from tree", item);
}

TEST(remove)
{
	int item = 0;
	int key = 0;
	CITreeError err;
	size_t size;

	/* Tree is empty, delete should return empty error. 
	 */
	err = CITree_Delete(&tree->citree, &item, 0);
	ASSERT(err == CITREE_ERR_EMPTY, "No empty error deleting from epty tree, got %d", err);

	/* Insert an element, then assert tree is empty after removing it.
	 */
	item = 1;
	CITree_Push(&tree->citree, &item, &key);
	item = 0;
	err = CITree_Delete(&tree->citree, &item, 0);
	size = CITree_Size(&tree->citree);
	ASSERT(err == CITREE_OK, "Got err %d deleting tree of size one", err);
	ASSERT(item == 1, "Removed wrong item from tree, got %d", item);
	ASSERT(size == 0, "Tree should be size zero, not %zu", size);

	/* Build a tree to test a dimension of heapify. The tree is build like this:
	 *       0:0
	 *        ^
	 *       / \
	 *    1:4   2:7
	 *     ^
	 *    / \
	 * 3:6   4:5
	 * where v:k = value:key                         
	 * 0:0 is removed, 4:5 should replace 1:4 and 1:4 replace 0:0.
	 */
	item = 0;
	key = 0;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 4;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 7;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 6;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 5;
	CITree_Push(&tree->citree, &item, &key);

	/* When we remove the root now, key 5 should be heapified
	 * to a child of the root with one child.
	 */
	err = CITree_Delete(&tree->citree, &item, 0);
	ASSERT(err == CITREE_OK, "heapify test 1: incorrectly got err %d", err);
	ASSERT(item == 0, "heapify test 1: incorrectly deleted item value %d", item);

	/* index 1 should now have pair 4:5
	 */
	CITree_Get(&tree->citree, &item, 1);
	ASSERT(item == 4," heapify test 1: pair 4:5 item value is incorrectly %d", item);

	/* index 4 (originally pair 4:5) should be empty.
	 */
	err = CITree_Get(&tree->citree, &item, 4);
	ASSERT(err == CITREE_ERR_EMPTY, "heapify test 1: no empty error at index 4, got %d", err);

	/* Clear the tree for next heapify test. 
	 */
	CITree_Clear(&tree->citree);

	/* Test another dimension of heapify. The tree will is build like this:
	 *           0:1
	 *        ___/ \___
	 *       /         \
	 *    1:6          2:7
	 *    / \          /
	 * 3:9   4:10   5:8
	 *
	 * 0:1 is removed, 5:8 should replace 1:6, and 1:6 replace 0:1.
	 */
	item = 0;
	key = 1;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 6;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 7;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 9;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 10;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 8;
	CITree_Push(&tree->citree, &item, &key);

	/* When we remove 0:1, we should see 5:8 be moved to index 1 (original
	 * location of 1:6).
	 */
	err = CITree_Delete(&tree->citree, &item, 0);
	ASSERT(err == CITREE_OK, "heapify test 2: incorrectly got err %d", err);
	ASSERT(item == 0, "heapify test 2: incorrectly deleted item value %d", item);

	/* index 0 should now have 1:6
	 */
	CITree_Get(&tree->citree, &item, 0);
	ASSERT(item == 1," heapify test 2: pair 1:6 item value is incorrectly %d", item);

	/* index 1 should now have 5:8
	 */
	CITree_Get(&tree->citree, &item, 1);
	ASSERT(item == 5," heapify test 2: pair 5:8 item value is incorrectly %d", item);

	/* index 5 should be empty now.
	 */
	err = CITree_Get(&tree->citree, &item, 5);
	ASSERT(err == CITREE_ERR_EMPTY, "heapify test 2: no empty error at index 5, got %d", err);

	/* Clear the tree for next heapify test. 
	 */
	CITree_Clear(&tree->citree);

	/* Test a third dimension of heapify. The tree built will look like this:
	 *         0:1
	 *        _/ \_
	 *       /     \
	 *    1:4       2:7
	 *    / \
	 * 3:6   4:9
	 *
	 * 0:1 is removed, 4:9 replaces 3:6, 3:6 replaces 1:4, 1:4 replaces 0:1.
	 */
	item = 0;
	key = 1;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 4;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 7;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 6;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 9;
	CITree_Push(&tree->citree, &item, &key);

	/* Remove 0:1 now.
	 */		
	err = CITree_Delete(&tree->citree, &item, 0);
	ASSERT(err == CITREE_OK, "heapify test 3: incorrectly got err %d", err);
	ASSERT(item == 0, "heapify test 3: incorrectly deleted item value %d", item);

	/* index 0 should now contain 1:5.
	 */
	CITree_Get(&tree->citree, &item, 0);
	ASSERT(item == 1," heapify test 3: pair 1:5 item value is incorrectly %d", item);
	
	/* index 1 should now contain 3:6.
	 */
	CITree_Get(&tree->citree, &item, 1);
	ASSERT(item == 3," heapify test 3: pair 3:6 item value is incorrectly %d", item);
	
	/* index 3 should now contain 4:9.
	 */
	CITree_Get(&tree->citree, &item, 3);
	ASSERT(item == 4," heapify test 3: pair 4:9 item value is incorrectly %d", item);
	
	/* index 4 should now be empty.
	 */
	err = CITree_Get(&tree->citree, &item, 4);
	ASSERT(err == CITREE_ERR_EMPTY, "heapify test 3: no empty error at index 4, got %d", err);

	/* Clear the tree for next heapify test. 
	 */
	CITree_Clear(&tree->citree);

	/* Test a fourth dimension of heapify. The tree built will look like this:
	 *           0:1
	 *        ___/ \___
	 *       /         \
	 *    1:6          2:7
	 *    / \          /
	 * 3:9   4:10   5:11
	 *
	 * 0:1 is removed, 1:6 replaces 0:1, 3:9 replaces 1:6, 5:11 replaces 3:9
	 */
	item = 0;
	key = 1;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 6;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 7;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 9;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 10;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 11;
	CITree_Push(&tree->citree, &item, &key);

	/* Remove 0:1 now.
	 */		
	err = CITree_Delete(&tree->citree, &item, 0);
	ASSERT(err == CITREE_OK, "heapify test 4: incorrectly got err %d", err);
	ASSERT(item == 0, "heapify test 4: incorrectly deleted item value %d", item);

	/* index 0 should now contain 1:6.
	 */
	CITree_Get(&tree->citree, &item, 0);
	ASSERT(item == 1," heapify test 4: pair 1:6 item value is incorrectly %d", item);
	
	/* index 1 should now contain 3:9.
	 */
	CITree_Get(&tree->citree, &item, 1);
	ASSERT(item == 3," heapify test 4: pair 3:9 item value is incorrectly %d", item);
	
	/* index 3 should now contain 5:11.
	 */
	CITree_Get(&tree->citree, &item, 3);
	ASSERT(item == 5," heapify test 4: pair 5:11 item value is incorrectly %d", item);
	
	/* index 5 should now be empty.
	 */
	err = CITree_Get(&tree->citree, &item, 5);
	ASSERT(err == CITREE_ERR_EMPTY, "heapify test 4: no empty error at index 5, got %d", err);
}

TEST(normal_operation)
{
	int i;
	int item;
	int key;

	/* Fill the tree up. 
	 */
	for( i = 0; i < TREE_MAX_SIZE; ++i ) {
		item = i;
		key = TREE_MAX_SIZE - i;
		CITree_Push(&tree->citree, &item, &key);
	}

	/* Empty the tree. 
	 */
	for( i = 0; i < TREE_MAX_SIZE; ++i ) {
		CITree_Pop(&tree->citree, &item);
		ASSERT(item == TREE_MAX_SIZE - i - 1,
		       "pop %d gave item %d, when expecting %d",
		       i, item, TREE_MAX_SIZE - i - 1);
	}

	/* Repeat above, except fill up the tree with keys such
	 * that heapify will never have to run.
	 */
	for( i = 0; i < TREE_MAX_SIZE; ++i ) {
		item = i;
		key = i;
		CITree_Push(&tree->citree, &item, &key);
	}

	/* Empty the tree. 
	 */
	for( i = 0; i < TREE_MAX_SIZE; ++i ) {
		CITree_Pop(&tree->citree, &item);
		ASSERT(item == i,
		       "pop %d gave item %d, when expecting %d",
		       i, item, i);
	}
}

TEST(delete_element)
{
	int item;
	int key;
	CITreeError err;

	/* Test with empty tree.
	 */
	item = 0;
	err = CITree_DeleteElement(&tree->citree, &item);
	ASSERT(err == CITREE_ERR_EMPTY, "Wrong error code returned");
	
	/* Construct a tree with value:key pairs:
	 *       0:0
	 *        ^
	 *       / \
	 *    1:4   2:7
	 *     ^
	 *    / \
	 * 3:6   4:5
	 * Then, remove 1:4 by value.
	 */
	item = 0;
	key = 0;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 4;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 7;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 6;
	CITree_Push(&tree->citree, &item, &key);
	++item;
	key = 5;
	CITree_Push(&tree->citree, &item, &key);

	/* Test remove with element thats not in the tree
	 */
	item = 2000;
	err = CITree_DeleteElement(&tree->citree, &item);
	ASSERT(err == CITREE_ERR_EMPTY, "Got wrong error return");

	/* Removing 1:4 to get tree that looks like:
	 *       0:0
	 *        ^
	 *       / \
	 *    1:4   2:7
	 *     ^
	 *    /
	 * 3:6
	 */
	item = 4;
	err = CITree_DeleteElement(&tree->citree, &item);
	ASSERT(err == CITREE_OK, "Should not get error return");

	CITree_Get(&tree->citree, &item, 0);
	ASSERT(item == 0, "Tree did not heapify correctly at 0, got %d", item);
	CITree_Get(&tree->citree, &item, 1);
	ASSERT(item == 1, "Tree did not heapify correctly at 1, got %d", item);
	CITree_Get(&tree->citree, &item, 2);
	ASSERT(item == 2, "Tree did not heapify correctly at 2, got %d", item);
	CITree_Get(&tree->citree, &item, 3);
	ASSERT(item == 3, "Tree did not heapify correctly at 3, got %d", item);
	ASSERT(CITree_Size(&tree->citree) == 4, "Tree is incorrect size");
}

TEST_SUITE(binary_tree)
{
	tree = &dynamic_tree;
	
	ADD_TEST(push);
	ADD_TEST(get);
	ADD_TEST(remove);
	ADD_TEST(normal_operation);
	ADD_TEST(delete_element);
}
