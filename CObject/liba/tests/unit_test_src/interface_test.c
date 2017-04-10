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
 * This test suite is used to verify virtual functions inherited from interfaces
 * can be correctly implemented and overrode.
 */

#include <test_classes/interface_test_classes.h>
#include <unit.h>

/* Setup and teardown unused. */
TEST_SETUP( ) { }
TEST_TEARDOWN( ) { }

TEST(implementing)
{
	struct ITClassA class;

	newITClassA(&class);

	ASSERT(ITInterface2_Method0(&class.itInterface2) == IT_CLASSA_I2_METHOD0, "Failed to run I2 M0");
	ASSERT(ITInterface2_Method1(&class.itInterface2) == IT_CLASSA_I2_METHOD1, "Failed to run I2 M1");

	CDestroy(&class);
}

TEST(interface_inheritance)
{
	struct ITClassA class;

	newITClassA(&class);

	ASSERT(ITInterface1_Method0(&class.itInterface1) == IT_CLASSA_I1_METHOD0, "Failed to run I1 M0");
	ASSERT(ITInterface0_Method0(&class.itInterface1.itInterface0) == IT_CLASSA_I0_METHOD0, "Failed to run I0 M0");
	ASSERT(ITInterface0_Method1(&class.itInterface1.itInterface0) == IT_CLASSA_I0_METHOD1, "Failed to run I0 M1");

	CDestroy(&class);
}

TEST(override)
{
	struct ITClassB class;

	newITClassB(&class);

	ASSERT(ITInterface2_Method0(&class.classA.itInterface2) == IT_CLASSA_I2_METHOD0 + IT_CLASSB_I2_METHOD0, "Failed to override I2 M0");
	ASSERT(ITInterface2_Method1(&class.classA.itInterface2) == IT_CLASSB_I2_METHOD1, "Failed to relink I2 M1");
	ASSERT(ITInterface1_Method0(&class.classA.itInterface1) == IT_CLASSA_I1_METHOD0, "Failed to run I1 M0");
	ASSERT(ITInterface0_Method0(&class.classA.itInterface1.itInterface0) == IT_CLASSA_I0_METHOD0 + IT_CLASSB_I0_METHOD0, "Failed to override I0 M0");
	ASSERT(ITInterface0_Method1(&class.classA.itInterface1.itInterface0) == IT_CLASSB_I0_METHOD1, "Failed to relink I0 M1");

	CDestroy(&class);
}

TEST(deep_override)
{
	struct ITClassC class;

	newITClassC(&class);

	ASSERT(ITInterface2_Method0(&class.classB.classA.itInterface2) == IT_CLASSA_I2_METHOD0 + IT_CLASSB_I2_METHOD0 + IT_CLASSC_I2_METHOD0, "Failed to override I2 M0");
	ASSERT(ITInterface2_Method1(&class.classB.classA.itInterface2) == IT_CLASSB_I2_METHOD1, "Failed to relink I2 M1");
	ASSERT(ITInterface1_Method0(&class.classB.classA.itInterface1) == IT_CLASSA_I1_METHOD0 + IT_CLASSC_I1_METHOD0, "Failed to run I1 M0");
	ASSERT(ITInterface0_Method0(&class.classB.classA.itInterface1.itInterface0) == IT_CLASSA_I0_METHOD0 + IT_CLASSB_I0_METHOD0 + IT_CLASSC_I0_METHOD0, "Failed to override I0 M0");
	ASSERT(ITInterface0_Method1(&class.classB.classA.itInterface1.itInterface0) == IT_CLASSB_I0_METHOD1, "Failed to relink I0 M1");

	CDestroy(&class);
}

TEST_SUITE(interface_suite)
{
	ADD_TEST(implementing);
	ADD_TEST(interface_inheritance);
	ADD_TEST(override);
	ADD_TEST(deep_override);
}
