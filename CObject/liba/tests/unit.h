/* Copyright (C) 2015  Brendan Bruner
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
 *
 * This test suite was inspired from the test suite at:
 * http://www.jera.com/techinfo/jtns/jtn002.html - January 2015
 */

#ifndef UNIT_H_
#define UNIT_H_

#include <stdio.h>

#define UNIT_PRINT( ... ) 	printf( __VA_ARGS__ )
#define UNIT_FLUSH( )		fflush(stdout)

extern unsigned int unit_asserts_passed, delta_unit_asserts_passed;
extern unsigned int unit_asserts_failed, delta_unit_asserts_failed;
extern unsigned int unit_tests_run;
extern unsigned int unit_tests_aborted;
extern unsigned int unit_asserts_failed_pre_test;
extern unsigned int unit_tests_passed, unit_tests_failed;

#define FAIL_TEST				0
#define PASS_TEST				1

#define TEST(name)			static void test_##name (void)
#define TEST_SETUP( )			static void _unit_test_setup( )
#define TEST_TEARDOWN( )		static void _unit_test_teardown( )

#define TEST_SUITE(suite) 		void all_tests_##suite (void)

#define ABORT_TEST(...)							\
	do {								\
		UNIT_PRINT( "\t\tABORTED: " );				\
		UNIT_PRINT(__VA_ARGS__ );				\
		UNIT_PRINT( "\n" );					\
		++unit_tests_aborted;					\
		++unit_tests_failed;					\
		return;							\
	} while( 0 )

#define ASSERT(test, ...)						\
	do {								\
		if( !(test) ) { 					\
			UNIT_PRINT( "\t\tFAILURE: %d\n\t\t", __LINE__ );\
			UNIT_PRINT(__VA_ARGS__ );			\
			UNIT_PRINT( "\n" );				\
			UNIT_FLUSH( );					\
			++unit_asserts_failed;				\
		}							\
		else {							\
			++unit_asserts_passed;				\
		}							\
	} while(0)

#define ADD_TEST(name)							\
	do {								\
		UNIT_PRINT( "\tTest: %s...\n", #name );			\
		_unit_test_setup( );					\
		unit_asserts_failed_pre_test = unit_asserts_failed; 	\
		test_##name (); 					\
		_unit_test_teardown( );					\
		if( unit_asserts_failed > unit_asserts_failed_pre_test ) {	\
			unit_tests_failed += 1;					\
		}								\
		else {								\
			unit_tests_passed += 1;					\
		}								\
		++unit_tests_run;						\
	} while(0)

#define RUN_TEST_SUITE(suite)						\
	  do {								\
		UNIT_PRINT("Running test suite: %s...\n", #suite);	\
		delta_unit_asserts_passed = unit_asserts_passed;	\
		delta_unit_asserts_failed = unit_asserts_failed;	\
		all_tests_##suite ();					\
		UNIT_PRINT("Asserts passed: %d\nAsserts failed: %d\n\n",\
			unit_asserts_passed - delta_unit_asserts_passed,	\
			unit_asserts_failed - delta_unit_asserts_failed);	\
	} while(0)

#define PRINT_DIAG()							\
	do {								\
		UNIT_PRINT("DIAGNOSTICS...\n");				\
		UNIT_PRINT("\tTests passed:\t%d\n"			\
			"\tTests failed:\t%d\n"				\
			"\tAsserts passed:\t%d\n"			\
			"\tAsserts failed:\t%d\n"			\
			"\tAsserts made:\t%d\n"				\
			"\tTest aborted:\t%d\n"				\
			"\tTests run:\t%d\n",				\
			unit_tests_passed,				\
			unit_tests_failed,				\
			unit_asserts_passed,				\
			unit_asserts_failed,				\
			unit_asserts_passed + unit_asserts_failed, 	\
			unit_tests_aborted,				\
			unit_tests_run );				\
	} while( 0 )


#endif /* UNIT_H_ */
