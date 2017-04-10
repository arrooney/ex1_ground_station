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
 */

#include <unit.h>

extern TEST_SUITE(destructor_suite);
extern TEST_SUITE(virtual_suite);
extern TEST_SUITE(interface_suite);
extern TEST_SUITE(array_queue);
extern TEST_SUITE(array_list);
extern TEST_SUITE(array_list_iterator);
extern TEST_SUITE(binary_tree);
extern TEST_SUITE(threaded_array_queue);
extern TEST_SUITE(soft_serial);

int main( int argc, char** argv )
{
	(void) argc; (void) argv;
	RUN_TEST_SUITE(destructor_suite);
	RUN_TEST_SUITE(virtual_suite);
	RUN_TEST_SUITE(interface_suite);
	RUN_TEST_SUITE(array_queue);
	RUN_TEST_SUITE(array_list);
	RUN_TEST_SUITE(array_list_iterator);
	RUN_TEST_SUITE(binary_tree);
	RUN_TEST_SUITE(soft_serial);
	RUN_TEST_SUITE(threaded_array_queue);
	PRINT_DIAG( );
	return 0;
}
