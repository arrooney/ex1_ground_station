/*
 * Copyright 2017 Brendan Bruner
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
 * April 11, 2017
 */

#include <string.h>
#include <unit.h>
#include <CCSoftSerialDev.h>

#define TEST_CHAR_1 'a'
#define TEST_CHAR_2 'z'
#define TEST_MSG "test message\n"

extern struct CCSoftSerialDev io_master;

TEST_SETUP( )
{
}

TEST_TEARDOWN( )
{
}

TEST( printf )
{
	int bytes_printed;
	
	CCSoftSerialDev_Select(&io_master, IO_SLAVE_ID, COS_BLOCK_FOREVER);
	bytes_printed = printf(TEST_MSG);
	ASSERT(bytes_printed == (int) strlen(TEST_MSG), "Printed wrong amount of bytes, %d", bytes_printed);
}

TEST( getchar )
{
	char msg;

	msg = TEST_CHAR_1;
	CCSoftSerialDev_Select(&io_master, IO_SLAVE_ID, COS_BLOCK_FOREVER);
	CCSoftSerialDev_Write(&io_master, &msg, COS_BLOCK_FOREVER);
	msg = TEST_CHAR_2;
	msg = getchar( );
	ASSERT(msg == TEST_CHAR_1, "Got wrong char, %c", msg);
}

TEST_SUITE(iohook)
{
	ADD_TEST(printf);
	ADD_TEST(getchar);
}
