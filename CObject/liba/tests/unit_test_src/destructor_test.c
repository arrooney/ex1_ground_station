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
 * Jun 19, 2016
 *
 * This test suite is used to verify the destructor function. After an object is initialized, a
 * memory free hook can be set that must be  called by the destructor. This will verify the
 * destructor correctly calls the hook.
 *
 * In addition, it will verify the destructor can be overriden and is called in the correct cascading
 * order as the object is destroyed.
 */

#include <test_classes/destructor_test_classes.h>
#include <unit.h>

/* Setup and teardown unused. */
TEST_SETUP( ) { }
TEST_TEARDOWN( ) { }

#define FREE_USED 0
#define FREE_UNUSED 1

static int freeUsed = FREE_UNUSED;

static void testFree( void* size )
{
	(void) size;
	freeUsed = FREE_USED;
}


TEST(free_hook)
{
	struct DTClassA classA;
	int temp;

	/* Construct and set free hook for destructor. */
	newDTClassA(&classA, &temp);

	freeUsed = FREE_UNUSED;
	CFreeWith(&classA, testFree);

	/* Test calling the destructor calls the free method we set. */
	CDestroy(&classA);

	ASSERT(freeUsed == FREE_USED, "Failed to call free( ) hook");
}

TEST(inheritance_free_hook)
{
	struct DTClassB class;
	int temp;

	/* Construct and set free hook for destructor. */
	newDTClassB(&class, &temp);

	freeUsed = FREE_UNUSED;
	CFreeWith(&class, testFree);

	/* Test calling the destructor calls the free method we set. */
	CDestroy(&class);

	ASSERT(freeUsed == FREE_USED, "Failed to call free( ) hook");
}

TEST(override_destructor)
{
	struct DTClassC class;
	int temp;

	/* Construct and set free hook for destructor. */
	newDTClassC(&class, &temp);

	freeUsed = FREE_UNUSED;
	CFreeWith(&class, testFree);

	/* Assert value of temp before destruction. */
	ASSERT(temp == DT_CLASS_A_VAL, "Failed to init test var");

	/* Test calling the destructor calls the free method we set */
	/* and cascades the destructor. */
	CDestroy(&class);

	ASSERT(temp == DT_CLASS_A_VAL + 1, "Failed to cascade destructor - ie - override it");
	ASSERT(freeUsed == FREE_USED, "Failed to call free( ) hook");
}

TEST(gap_override)
{
	struct DTClassD class;
	int temp;

	/* Construct and set free hook for destructor. */
	newDTClassD(&class, &temp);

	freeUsed = FREE_UNUSED;
	CFreeWith(&class, testFree);

	/* Assert value of temp before destruction. */
	ASSERT(temp == DT_CLASS_A_VAL, "Failed to init test var");

	/* Test calling the destructor calls the free method we set */
	/* and cascades the destructor. */
	CDestroy(&class);

	ASSERT(temp == DT_CLASS_A_VAL + 1, "Failed to cascade destructor - ie - override it");
	ASSERT(freeUsed == FREE_USED, "Failed to call free( ) hook");
}

TEST(deep_override)
{
	struct DTClassE class;
	int temp;

	/* Construct and set free hook for destructor. */
	newDTClassE(&class, &temp);

	freeUsed = FREE_UNUSED;
	CFreeWith(&class, testFree);

	/* Assert value of temp before destruction. */
	ASSERT(temp == DT_CLASS_A_VAL, "Failed to init test var");

	/* Test calling the destructor calls the free method we set */
	/* and cascades the destructor in the correct order. */
	CDestroy(&class);

	ASSERT(temp == DT_CLASS_E_VAL+1, "Failed to cascade destructor in correct order - %d", temp);
	ASSERT(freeUsed == FREE_USED, "Failed to call free( ) hook");
}

TEST_SUITE(destructor_suite)
{
	ADD_TEST(free_hook);
	ADD_TEST(inheritance_free_hook);
	ADD_TEST(override_destructor);
	ADD_TEST(gap_override);
	ADD_TEST(deep_override);
}
