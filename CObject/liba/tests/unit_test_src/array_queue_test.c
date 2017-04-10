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
 * Jun 25, 2016
 */

#include <unit.h>
#include <CCArrayQueue.h>

TEST_SETUP( ) { }
TEST_TEARDOWN( ) { }

#define DEFAULT_LENGTH 12
#define DEFAULT_ELEMENT_SIZE 7

struct CCArrayQueue queue;

TEST(underflow)
{
	CIQueueError 	err;
	size_t		queue_size;
	char		test_element[DEFAULT_ELEMENT_SIZE];

	/* Assert size of new queue is zero. */
	queue_size = CIQueue_Size(&queue.ciqueue);
	ASSERT(queue_size == 0, "Queue should be size zero, not %zu", queue_size);

	/* Assert removing from an empty queue returns underflow error. */
	err = CIQueue_Remove(&queue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_ERR_UNDERFLOW, "No underflow error on remove, got %d instead", err);

	/* Assert peeking at an empty queue returns underflow error. */
	err = CIQueue_Peek(&queue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_ERR_UNDERFLOW, "No underflow error on peek, got %d instead", err);
}

TEST(middle)
{
	CIQueueError 		err;
	size_t				queue_size;
	char				test_element[DEFAULT_ELEMENT_SIZE];
	int i;

	/* Fill in test element. */
	for( i = 0; i < DEFAULT_ELEMENT_SIZE; ++i ) {
		test_element[i] = (char) i;
	}

	/* Insert element and assert queue size increases. */
	err = CIQueue_Insert(&queue.ciqueue, test_element);
	queue_size = CIQueue_Size(&queue.ciqueue);
	ASSERT(err == CIQUEUE_OK, "error on first insert, got %d instead of OK", err);
	ASSERT(queue_size == 1, "Queue should be size one after insert, not %zu", queue_size);

	/* Fill in test element with new data for inserting a second element. */
	for( i = 0; i < DEFAULT_ELEMENT_SIZE; ++i ) {
		test_element[i] = (char) (i + DEFAULT_ELEMENT_SIZE);
	}

	/* Insert element and assert queue size increases. */
	err = CIQueue_Insert(&queue.ciqueue, test_element);
	queue_size = CIQueue_Size(&queue.ciqueue);
	ASSERT(err == CIQUEUE_OK, "error on second insert, got %d instead of OK", err);
	ASSERT(queue_size == 2, "Queue should be size two after second insert, not %zu", queue_size);

	/* Peek first element and assert elements of peeked item. */
	err = CIQueue_Peek(&queue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_OK, "error on first peek, got %d instead of OK", err);
	for( i = 0; i < DEFAULT_ELEMENT_SIZE; ++i ) {
		ASSERT(test_element[i] == i, "element %d is %d, instead of %d", i, test_element[i], i);
	}

	/* Zero out test element for remove test. */
	for( i = 0; i < DEFAULT_ELEMENT_SIZE; ++i ) {
		test_element[i] = 0;
	}
	
	/* Get first element and assert elements of removed item. */
	err = CIQueue_Remove(&queue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_OK, "error on first remove, got %d instead of OK", err);
	for( i = 0; i < DEFAULT_ELEMENT_SIZE; ++i ) {
		ASSERT(test_element[i] == i, "element %d is %d, instead of %d", i, test_element[i], i);
	}
	/* Assert new queue size. */
	queue_size = CIQueue_Size(&queue.ciqueue);
	ASSERT(queue_size == 1, "Queue should be size one after remove, not %zu", queue_size);

	/* Peek second element and assert elements of peeked item. */
	err = CIQueue_Peek(&queue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_OK, "error on second peek, got %d instead of OK", err);
	for( i = 0; i < DEFAULT_ELEMENT_SIZE; ++i ) {
		ASSERT(test_element[i] == i + DEFAULT_ELEMENT_SIZE, "element %d is %d, instead of %d", i, test_element[i], i);
	}
	
	/* Zero out test element for remove. */
	for( i = 0; i < DEFAULT_ELEMENT_SIZE; ++i ) {
		test_element[i] = 0;
	}
	
	/* Get second element and assert new queue size. */
	err = CIQueue_Remove(&queue.ciqueue, test_element);
	queue_size = CIQueue_Size(&queue.ciqueue);
	ASSERT(err == CIQUEUE_OK, "error on second remove, got %d instead of OK", err);	
	ASSERT(queue_size == 0, "Queue should be size one after second remove, not %zu", queue_size);
	/* Assert elements of removed item. */
	for( i = 0; i < DEFAULT_ELEMENT_SIZE; ++i ) {
		ASSERT(test_element[i] == i + DEFAULT_ELEMENT_SIZE, "element %d is %d, instead of %d", i, test_element[i], i + DEFAULT_ELEMENT_SIZE);
	}

	/* Assert underflow error is still present after filling up then emptying the queue. */
 	err = CIQueue_Peek(&queue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_ERR_UNDERFLOW, "no underflow error on peek, got %d instead", err);
	err = CIQueue_Remove(&queue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_ERR_UNDERFLOW, "no underflow error on remove, got %d instead", err);
}

TEST(overflow)
{
	CIQueueError 		err;
	size_t				queue_size;
	unsigned char		test_element[DEFAULT_ELEMENT_SIZE];
	size_t i, j;

	/* fill up the queue. */
	for( i = 0; i < DEFAULT_LENGTH; ++i ) {
		for( j = 0; j < DEFAULT_ELEMENT_SIZE; ++j ) {
			test_element[j] = (unsigned char) (i + j);
		}
		err = CIQueue_Insert(&queue.ciqueue, test_element);
		ASSERT(err == CIQUEUE_OK, "Error inserting, got %d during insert %zu, instead of %d", err, i, CIQUEUE_OK);

		/* Assert queue size. */
		queue_size = CIQueue_Size(&queue.ciqueue);
		ASSERT(queue_size == i + 1, "Queue should be size %zu after %zu'th insert, not %zu", i + 1, i, queue_size);
	}

	/* This insert should give overflow error. */
	err = CIQueue_Insert(&queue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_ERR_OVERFLOW, "No overflow error, got %d instead of %d", err, CIQUEUE_ERR_OVERFLOW);

	/* Remove back to size zero. */
	for( i = 0; i < DEFAULT_LENGTH; ++i ) {

		/* Test peek. */
		err = CIQueue_Peek(&queue.ciqueue, test_element);
		ASSERT(err == CIQUEUE_OK, "Error peeking, got %d during removal %zu, instead of %d", err, i, CIQUEUE_OK);
		for( j = 0; j < DEFAULT_ELEMENT_SIZE; ++j ) {
			ASSERT(test_element[j] == i + j, "Peeked item is incorrect during removal %zu, got %d instead of %zu", i, test_element[j], i + j);
		}

		/* Zero out test element. */
		for( j = 0; j < DEFAULT_ELEMENT_SIZE; ++j ) {
			test_element[j] = 0;
		}

		/* Test remove. */
		err = CIQueue_Remove(&queue.ciqueue, test_element);
		ASSERT(err == CIQUEUE_OK, "Error removing, got %d during removal %zu, instead of %d", err, i, CIQUEUE_OK);
		for( j = 0; j < DEFAULT_ELEMENT_SIZE; ++j ) {
			ASSERT(test_element[j] == i + j, "Removed item is incorrect during removal %zu, got %d instead of %zu", i, test_element[j], i + j);
		}

		/* Assert new queue size. */
		queue_size = CIQueue_Size(&queue.ciqueue);
		ASSERT(queue_size == DEFAULT_LENGTH - i - 1, "Queue should be size %zu after %zu'th remove, not %zu", DEFAULT_LENGTH - i - 1, i, queue_size);
	}

	/* Should get underflow error now. */
	err = CIQueue_Peek(&queue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_ERR_UNDERFLOW, "No underflow error on peek, got %d instead of %d", err, CIQUEUE_ERR_UNDERFLOW);
	err = CIQueue_Remove(&queue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_ERR_UNDERFLOW, "No underflow error on remove, got %d instead of %d", err, CIQUEUE_ERR_UNDERFLOW);
	/* Assert new queue size. */
	queue_size = CIQueue_Size(&queue.ciqueue);
	ASSERT(queue_size == 0, "Queue should be size zero after underflow, not %zu", queue_size);
}

TEST(null_remove)
{
	CIQueueError	err;
	unsigned char	test_element[DEFAULT_ELEMENT_SIZE];

	/* Insert element so null remove doesn't fail. */
	if( CIQueue_Insert(&queue.ciqueue, test_element) != CIQUEUE_OK ) {
		ABORT_TEST("Failed to insert");
	}

	/* Do null remove. */
	err = CIQueue_Remove(&queue.ciqueue, NULL);
	ASSERT(err == CIQUEUE_OK, "Failed to do NULL remove with err %d", err);
}

TEST(clear)
{
	char test_element[DEFAULT_ELEMENT_SIZE];
	size_t size;
	
	/* Add some elements, then assert queue size is zero. */
	CIQueue_Insert(&queue.ciqueue, test_element);
	CIQueue_Insert(&queue.ciqueue, test_element);

	CIQueue_Clear(&queue.ciqueue);
	size = CIQueue_Size(&queue.ciqueue);
	ASSERT(size == 0, "Failed to clear queue");
}

TEST(one)
{
	#define ONE_TEST_QUEUE_LEN 1
	struct CCArrayQueue oneQueue;
	int i;
	char test_element[DEFAULT_ELEMENT_SIZE];
	CIQueueError err;
	size_t size;
	
	if( CCArrayQueue(&oneQueue, DEFAULT_ELEMENT_SIZE, ONE_TEST_QUEUE_LEN) != COBJ_OK ) {
		ABORT_TEST("Failed to construct queue");
	}

	/* Insert. */
	for( i = 0; i < DEFAULT_ELEMENT_SIZE; ++i ) {
		test_element[i] = i;
	}
	err = CIQueue_Insert(&oneQueue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_OK, "Error %d inserting into queue of size one", err);

	/* Second insert should fail. */
	test_element[0] = i;
	err = CIQueue_Insert(&oneQueue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_ERR_OVERFLOW, "No overflow error, got err %d instead", err);

	/* Peek should work */
	err = CIQueue_Peek(&oneQueue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_OK, "Error %d on peek", err);
	ASSERT(test_element[0] == 0, "Queue returned incorrect item on peek");

	/* Remove should work too. */
	test_element[0] = i;
	err = CIQueue_Remove(&oneQueue.ciqueue, test_element);
	size = CIQueue_Size(&oneQueue.ciqueue);
	ASSERT(err == CIQUEUE_OK, "Error %d on remove", err);
	ASSERT(test_element[0] == 0, "Queue returned incorrect item on remove");
	ASSERT(size == 0, "Queue size didn't decrement on remove, got %zu", size);

	/* Peek and remove should fail now. */
	err = CIQueue_Peek(&oneQueue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_ERR_UNDERFLOW, "No undeflow error after emptying queue on peek");
	err = CIQueue_Remove(&oneQueue.ciqueue, test_element);
	ASSERT(err == CIQUEUE_ERR_UNDERFLOW, "No undeflow error after emptying queue on remove");

	CDestroy(&oneQueue);
}

TEST_SUITE(array_queue)
{
	/* Construct queue. */
	if( CCArrayQueue(&queue, DEFAULT_ELEMENT_SIZE, DEFAULT_LENGTH) != COBJ_OK ) {
		ABORT_TEST("Failed to construct queue");
	}

	/* Test underflow error is returned on empty queue. */
	ADD_TEST(underflow);
	/* Assert standard insert and remove operations. */
	ADD_TEST(middle);
	/* Test overflow error is returned on full queue. */
	ADD_TEST(overflow);
	/* Test we can remove from a queue with copying data out of it. */
	ADD_TEST(null_remove);
	/* Test we can clear a queue. */
	ADD_TEST(clear);
	/* Test a queue of size one. */
	ADD_TEST(one);

	CDestroy(&queue);
}
