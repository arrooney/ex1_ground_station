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
 * This test suite is used to verify virtual functions can be correctly overriden and
 * cascaded.
 */

#include <test_classes/virtual_test_classes.h>
#include <unit.h>

/* Setup and teardown unused. */
TEST_SETUP( ) { }
TEST_TEARDOWN( ) { }

TEST(base_virtual)
{
	struct VTClassA class;

	newVTClassA(&class);

	ASSERT(VTClassA_Method0(&class) == VT_CLASSA_METHOD0, "Virtual method 0 failed");
	ASSERT(VTClassA_Method1(&class) == VT_CLASSA_METHOD1, "Virtual method 1 failed");
	ASSERT(VTClassA_Method2(&class) == VT_CLASSA_METHOD2, "Virtual method 2 failed");
	ASSERT(VTClassA_Method3(&class) == VT_CLASSA_METHOD3, "Virtual method 3 failed");
	ASSERT(VTClassA_Method4(&class) == VT_CLASSA_METHOD4, "Virtual method 4 failed");

	CDestroy(&class);
}

TEST(override)
{
	struct VTClassB class;

	newVTClassB(&class);

	ASSERT(VTClassA_Method0(&class.classA) == VT_CLASSA_METHOD0, "Virtual method 0 failed");
	ASSERT(VTClassA_Method1(&class.classA) == VT_CLASSB_METHOD1, "Virtual method 1 failed");
	ASSERT(VTClassA_Method2(&class.classA) == VT_CLASSA_METHOD2 + VT_CLASSB_METHOD2, "Virtual method 2 failed");
	ASSERT(VTClassA_Method3(&class.classA) == VT_CLASSB_METHOD3, "Virtual method 3 failed");
	ASSERT(VTClassA_Method4(&class.classA) == VT_CLASSA_METHOD4 + VT_CLASSB_METHOD4, "Virtual method 4 failed");

	CDestroy(&class);
}

TEST(deep_override)
{
	struct VTClassC class;

	newVTClassC(&class);

	ASSERT(VTClassA_Method0(&class.classB.classA) == VT_CLASSA_METHOD0, "Virtual method 0 failed");
	ASSERT(VTClassA_Method1(&class.classB.classA) == VT_CLASSB_METHOD1, "Virtual method 1 failed");
	ASSERT(VTClassA_Method2(&class.classB.classA) == VT_CLASSA_METHOD2 + VT_CLASSB_METHOD2, "Virtual method 2 failed");
	ASSERT(VTClassA_Method3(&class.classB.classA) == VT_CLASSB_METHOD3 + VT_CLASSC_METHOD3, "Virtual method 3 failed");
	ASSERT(VTClassA_Method4(&class.classB.classA) == VT_CLASSA_METHOD4 + VT_CLASSB_METHOD4 + VT_CLASSC_METHOD4, "Virtual method 4 failed");

	CDestroy(&class);
}

TEST_SUITE(virtual_suite)
{
	ADD_TEST(base_virtual);
	ADD_TEST(override);
	ADD_TEST(deep_override);
}
