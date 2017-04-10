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
 * April 5, 2017
 */

#include <unit.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <CCSoftSerialBus.h>
#include <CCSoftSerialDev.h>

/* Cannot test arbitration unless the number is
 * three or greater.
 */
#if CCSOFTSERIAL_MAX_PENDING_MASTERS < 3
#error "Cannot perform soft serial tests. See this file for reason why"
#endif

#define TEST_MSG_1 'a'
#define TEST_TENTH_SECOND_MS (1000/10) /* one tenth of a second in milli seconds. */
#define TEST_TENTH_SECOND (1000*1000/10) /* one tenth of a second in micro seconds. */
#define TEST_TOTAL_MASTERS (CCSOFTSERIAL_MAX_PENDING_MASTERS+2)
#define TEST_MAX_PENDING_MASTERS CCSOFTSERIAL_MAX_PENDING_MASTERS
#define TEST_CHANNEL_LENGTH 10
#define TEST_SLAVE_ID (TEST_TOTAL_MASTERS+1)
#define TEST_MASTER_ID 0
static struct CCSoftSerialDev master[TEST_TOTAL_MASTERS];
static struct CCSoftSerialDev slave;
static struct CCSoftSerialBus bus;
static pthread_attr_t attr;

TEST_SETUP( )
{
	CError err;
	int i;
	int thread_err;

	thread_err = pthread_attr_init(&attr);
	if( thread_err != 0 ) {	
		UNIT_PRINT("Error creating attritube. "
			   "Test results are unreliable.");
	}
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	err = CCSoftSerialBus(&bus, sizeof(char), TEST_CHANNEL_LENGTH);
	if( err != COBJ_OK ) {
		UNIT_PRINT("Fatal error during test setup. "
			   "Test result are unreliable");
		UNIT_FLUSH();
	}
	
	for( i = 0; i < TEST_TOTAL_MASTERS; ++i ) {
		err = CCSoftSerialDevMaster(&master[i],
					    (unsigned char) i,
					    (CCSoftSerialDevID) i,
					    &bus);
		if( err != COBJ_OK ) {
			UNIT_PRINT("Fatal error during test setup. "
				   "Test result are unreliable");
			UNIT_FLUSH();
		}
	}
	err = CCSoftSerialDevSlave(&slave,
				   (CCSoftSerialDevID) TEST_SLAVE_ID,
				   &bus);
	if( err != COBJ_OK ) {
		UNIT_PRINT("Fatal error during test setup. "
			   "Test result are unreliable");
		UNIT_FLUSH();
	}
	
}

TEST_TEARDOWN( )
{
	int i;
	
	pthread_attr_destroy(&attr);
	
	for( i = 0; i < TEST_TOTAL_MASTERS; ++i ) {
		CDestroy(&master[i]);
	}
	CDestroy(&slave);
	CDestroy(&bus);
}

static void* arbitration_tree_fill( void* args )
{
	struct CCSoftSerialDev* device;

	device = args;
	CCSoftSerialDev_Select(device, TEST_SLAVE_ID, COS_BLOCK_FOREVER);
	CCSoftSerialDev_Unselect(device);
	pthread_exit(NULL);
}

static void* arbitration_priority( void* args )
{
	struct CCSoftSerialDev* device;

	device = args;
	CCSoftSerialDev_Select(device, TEST_SLAVE_ID, COS_BLOCK_FOREVER);
	pthread_exit(NULL);
}

static 	CCSoftSerialError arb_res_err;
static void* arbitration_reselection( void* args )
{
	struct CCSoftSerialDev* device;

	device = args;
	arb_res_err = CCSoftSerialDev_Isselected(device, COS_BLOCK_FOREVER);
	pthread_exit(&arb_res_err);
}

static void* arbitration_timeout( void* args )
{
	struct CCSoftSerialDev* device;
	CCSoftSerialError err;

	device = args;
	err = CCSoftSerialDev_Select(device, TEST_SLAVE_ID, TEST_TENTH_SECOND_MS);
	ASSERT(err == CCSOFTSERIAL_ERR_TIMEOUT, "Should have timed out");
	pthread_exit(&arb_res_err);
}

TEST(arbitration_max_masters)
{
	CCSoftSerialError err;
	pthread_t thread_handle[TEST_MAX_PENDING_MASTERS];
	int i, j;
	void* status;
	
	/* Test that a slave device cannot make a selection.
	 */
	err = CCSoftSerialDev_Select(&slave, TEST_MASTER_ID, COS_BLOCK_FOREVER);
	ASSERT(err == CCSOFTSERIAL_ERR_PRIV, "Slave cannot become bus master");

	/* Test for correct error code when attempting to select
	 * a device, but the maximum number of pending masters
	 * is already reached.
	 *
	 * To do this, we make master[0] take the bus. We create
	 * TEST_MAX_PENDING_MASTERS threads where each thread 
	 * blocks trying to take the bus with master[1..TEST_MAX_PENDING_MASTERS+1]
	 *
	 * Then, master[TEST_TOTAL_MASTERS-1] tries to take the bus. We test the
	 * return code on this.
	 *
	 * Finally, to clean things up, master[0] unsellects the bus. All threads
	 * will take their turn selecting then unselecting the bus and exit.
	 */
	err = CCSoftSerialDev_Select(&master[0], TEST_SLAVE_ID, COS_BLOCK_FOREVER);
	ASSERT(err == CCSOFTSERIAL_OK, "Should be no error when taking idle bus");

	/* Create the threads.
	 */
	for( i = 0; i < TEST_MAX_PENDING_MASTERS; ++i ) {
		err = pthread_create(&thread_handle[i], &attr, arbitration_tree_fill, &master[i+1]);
		if( err != 0 ) {
			CCSoftSerialDev_Unselect(&master[0]);
			for( j = 0; j <= i; ++j ) {
				pthread_join(thread_handle[j], &status);
			}
			ABORT_TEST("Error creating thread 1");
		}
	}

	/* This delay is used to give the new threads time to run and block
	 * on the bus.
	 */
	usleep(TEST_TENTH_SECOND);

	/* Test for the return code we're expecting.
	 */
	err = CCSoftSerialDev_Select(&master[TEST_TOTAL_MASTERS-1], TEST_SLAVE_ID, COS_BLOCK_FOREVER);
	ASSERT(err == CCSOFTSERIAL_ERR_OVRLD, "No error when too many masters are trying to take the bus");

	/* Clean up the threads
	 */
	ASSERT(CCSoftSerialDev_Isselected(&slave, COS_NO_BLOCK) == CCSOFTSERIAL_OK, "Slave should be selected");
	CCSoftSerialDev_Unselect(&master[0]);
	for( j = 0; j < TEST_MAX_PENDING_MASTERS; ++j ) {
		pthread_join(thread_handle[j], &status);
	}
}

TEST(arbitration_priority)
{
	CCSoftSerialError err;
	pthread_t thread_handle[TEST_MAX_PENDING_MASTERS];
	int thread_err;
	int i, j;
	void* status;

	/* Test that master priorities are correctly handled. First, we will
	 * use three masters:
	 *	* master[0], priority = 0
	 *	* master[1], priority = 1
	 *	* master[2], priority = 2
	 * 1. master[1] takes bus a seperate thread.
	 * 2. In a seperate thread, master[2] blocks on the bus.
	 * 3. In a seperate thread, master[0] blocks on the bus.
	 * 4. master[1]  releases bus
	 * 5. master[2] should take the bus now.
	 * 6. master[2] releases bus.
	 * 7. master[0] should take the bus now.
	 * 8. master[0] releases bus.
	 */
	CCSoftSerialDev_Select(&master[1], TEST_SLAVE_ID, COS_BLOCK_FOREVER);
	ASSERT(CCSoftSerialDev_Isselected(&slave, COS_NO_BLOCK) == CCSOFTSERIAL_OK, "Slave should be selected");
	
	/* Create threads for master[0,2] to take the bus.
	 */
	for( i = 0; i < 2; ++i ) {
		err = pthread_create(&thread_handle[2*i], &attr, arbitration_priority, &master[2*i]);
		if( err != 0 ) {
			CCSoftSerialDev_Unselect(&master[1]);
			for( j = 0; j <= i; ++j ) {
				pthread_join(thread_handle[j], &status);
			}
			ABORT_TEST("Error creating thread 2");
		}
	}

	/* This delay is used to give the new threads time to run and block
	 * on the bus.
	 */
	usleep(TEST_TENTH_SECOND);

	/* master[1] will release the bus, which means master[2] should take the
	 * bus.
	 */
	CCSoftSerialDev_Unselect(&master[1]);
	pthread_join(thread_handle[2], &status);
	thread_err = pthread_tryjoin_np(thread_handle[0], &status);
	ASSERT(CCSoftSerialDev_Isselected(&slave, COS_NO_BLOCK) == CCSOFTSERIAL_OK, "Slave should be selected");
	ASSERT(thread_err == EBUSY, "master[0] should no have taken the bus yet");

	/* master[2] currently has the bus. Release the bus and master[0] should take
	 * it.
	 */
	CCSoftSerialDev_Unselect(&master[2]);
	thread_err = pthread_join(thread_handle[0], &status);
	ASSERT(CCSoftSerialDev_Isselected(&slave, COS_NO_BLOCK) == CCSOFTSERIAL_OK, "Slave should be selected");
	ASSERT(thread_err == 0, "master[0] should have taken the bus");

	/* master[0] has bus, release it and finish the priority testing.
	 */
	CCSoftSerialDev_Unselect(&master[0]);
}

TEST(arbitration_reselection)
{
	CCSoftSerialError err;
	pthread_t thread_handle[TEST_MAX_PENDING_MASTERS];
	void* status;

	/* Test that reselection works as intended.
	 * 1. master[0] selects the slave
	 * 2. verify slave is selected
	 * 3. i seperate thread, master[1] blocks until it's selected
	 * 4. master[0] selects master[1]
	 * 5. verify master[1] got selected
	 */
	CCSoftSerialDev_Select(&master[0], TEST_SLAVE_ID, COS_NO_BLOCK);
	ASSERT(CCSoftSerialDev_Isselected(&slave, COS_NO_BLOCK) == CCSOFTSERIAL_OK, "Slave should be selected");

	/* Create thread for master[1] to block until selected.
	 */
	err = pthread_create(&thread_handle[0], &attr, arbitration_reselection, &master[1]);
	if( err != 0 ) {
		CCSoftSerialDev_Unselect(&master[0]);
		CCSoftSerialDev_Select(&master[0], CCSoftSerialDev_GetID(&master[1]), COS_BLOCK_FOREVER);
		pthread_join(thread_handle[0], &status);
		ABORT_TEST("Error creating thread 3");
	}

	/* This delay is used to give the new thread time to run and block
	 * on the bus.
	 */
	usleep(TEST_TENTH_SECOND);
	
	err = CCSoftSerialDev_Select(&master[0], CCSoftSerialDev_GetID(&master[1]), COS_NO_BLOCK);
	ASSERT(err == CCSOFTSERIAL_OK, "Should be no error doing a reselect");
	pthread_join(thread_handle[0], &status);
	ASSERT(CCSoftSerialDev_Isselected(&master[1], COS_NO_BLOCK) == CCSOFTSERIAL_OK, "Slave should be selected");
	ASSERT(*((CCSoftSerialError*) status) == CCSOFTSERIAL_OK, "Error in blocking thread");
	CCSoftSerialDev_Unselect(&master[0]);
}

TEST(arbitration_timeout)
{
	CCSoftSerialError err;
	pthread_t thread_handle[TEST_MAX_PENDING_MASTERS];
	void* status;

	/* Finally, we test that a device correctly removes itself from 
	 * the list of pending masters when a timeout occurs while trying
	 * to take the bus.
	 *
	 * 1. master[0] takes the bus
	 * 2. in seperate threads, master[1,2] take the bus
	 * 3. master[2] has a timeout. It will timeout while waiting to take bus
	 * 4. After timeout, master[0] releases bus.
	 * 5. master[1] should now take bus.
	 * 6. master[1] realeses bus.
	 */
	CCSoftSerialDev_Select(&master[0], TEST_SLAVE_ID, COS_BLOCK_FOREVER);

	/* Create threads for master[1,2] to block on bus.
	 */
	err = pthread_create(&thread_handle[0], &attr, arbitration_tree_fill, &master[1]);
	if( err != 0 ) {
		ABORT_TEST("Error creating thread 4");
	}
	err = pthread_create(&thread_handle[1], &attr, arbitration_timeout, &master[2]);
	if( err != 0 ) {
		CCSoftSerialDev_Unselect(&master[0]);
		pthread_join(thread_handle[0], &status);
		ABORT_TEST("Error creating thread 5");
	}
	
	/* Wait for master[2] to timeout.
	 */
	pthread_join(thread_handle[1], &status);

	/* Release bus, then wait for master[1] to take it.
	 */
	CCSoftSerialDev_Unselect(&master[0]);
	pthread_join(thread_handle[0], &status);

	/* If we're at this point, master[2] successfully removed itself from
	 * the tree of pending masters and master[1] was able to take the bus.
	 */
}

TEST(bus_release)
{
	CCSoftSerialError err;
	
	/* Release bus when not current bus master.
	 */
	err = CCSoftSerialDev_Unselect(&master[0]);
	ASSERT(err == CCSOFTSERIAL_ERR_PRIV, "Should not be able to release bus");

	/* Release bus when current owning it.
	 */
	CCSoftSerialDev_Select(&master[0], TEST_SLAVE_ID, COS_BLOCK_FOREVER);
	err = CCSoftSerialDev_Unselect(&master[0]);
	ASSERT(err == CCSOFTSERIAL_OK, "Could not release owned bus");
}

TEST(is_selected_current_master)
{
	CCSoftSerialError err;
	
	/* Assert return value of isselected() when
	 * current bus master.
	 */
	CCSoftSerialDev_Select(&master[0], TEST_SLAVE_ID, COS_BLOCK_FOREVER);
	err = CCSoftSerialDev_Isselected(&master[0], COS_BLOCK_FOREVER);
	ASSERT(err == CCSOFTSERIAL_OK, "Should return no error");
}

TEST(is_selected_current_slave)
{
	CCSoftSerialError err;
	
	/* Assert return value of isselected() when
	 * current bus slave.
	 */
	CCSoftSerialDev_Select(&master[0], TEST_SLAVE_ID, COS_BLOCK_FOREVER);
	err = CCSoftSerialDev_Isselected(&slave, COS_BLOCK_FOREVER);
	ASSERT(err == CCSOFTSERIAL_OK, "Should return no error");

}

TEST(is_selected_slave_timeout)
{
	CCSoftSerialError err;
	
	/* Assert return value of isselected() when
	 * timing out.
	 */
	err = CCSoftSerialDev_Isselected(&master[0], TEST_TENTH_SECOND_MS);
	ASSERT(err == CCSOFTSERIAL_ERR_TIMEOUT, "Should return no error");

}

static void* is_selected_do_selection( void* args )
{
	struct CCSoftSerialDev* device;

	device = args;

	/* Give owner thread time to do it's block 
	 * before making a selection.
	 */
	usleep(TEST_TENTH_SECOND);

	/* Do the selection.
	 */
	CCSoftSerialDev_Select(device, TEST_SLAVE_ID, COS_BLOCK_FOREVER);
	
	pthread_exit(NULL);
}

TEST(is_selected_blocking_slave)
{
	
	CCSoftSerialError err;
	int thread_err;
	pthread_t thread_handle;
	void* status;

	/* Create thread that will allow blocking slave
	 * to unblock.
	 */
	thread_err = pthread_create(&thread_handle, &attr, is_selected_do_selection, &master[0]);
	if( thread_err != 0 ) {
		pthread_join(thread_handle, &status);
		ABORT_TEST("Error creating thread");
	}

	/* Block, waiting until selected.
	 */
	err = CCSoftSerialDev_Isselected(&slave, COS_BLOCK_FOREVER);
	ASSERT(err == CCSOFTSERIAL_OK, "Should be selected..");

	pthread_join(thread_handle, &status);	
}

static void* data_transfer_read( void* args )
{
	struct CCSoftSerialDev* device;
	char msg;

	device = args;

	CCSoftSerialDev_Read(device, &msg, COS_BLOCK_FOREVER);
	ASSERT(msg == TEST_MSG_1, "Got wrong msg: %c", msg);
	pthread_exit(NULL);
}

TEST(data_transfer)
{
	CCSoftSerialError err;
	int thread_err;
	pthread_t thread_handle;
	void* status;
	char msg;
	
	msg = TEST_MSG_1;
	/* Test that unauthorized devices are blocked from reading/writing.
	 */
	err = CCSoftSerialDev_Write(&master[0], &msg, COS_BLOCK_FOREVER);
	ASSERT(err == CCSOFTSERIAL_ERR_PRIV, "Got wrong error: %d", err);

	err = CCSoftSerialDev_Read(&master[0], &msg, COS_BLOCK_FOREVER);
	ASSERT(err == CCSOFTSERIAL_ERR_PRIV, "Got wrong error: %d", err);
	
	/* Master writes and slave reads.
	 */
	CCSoftSerialDev_Select(&master[0], TEST_SLAVE_ID, COS_BLOCK_FOREVER);
	thread_err = pthread_create(&thread_handle, &attr, data_transfer_read, &slave);
	if( thread_err != 0 ) {
		pthread_join(thread_handle, &status);
		ABORT_TEST("Error creating thread");
	}

	err = CCSoftSerialDev_Write(&master[0], &msg, COS_BLOCK_FOREVER);
	pthread_join(thread_handle, &status);
	ASSERT(err == CCSOFTSERIAL_OK, "Got error writing: %d", err);

	/* Slave writes and master reads.
	 */
	thread_err = pthread_create(&thread_handle, &attr, data_transfer_read, &master[0]);
	if( thread_err != 0 ) {
		pthread_join(thread_handle, &status);
		ABORT_TEST("Error creating thread");
	}
	
	err = CCSoftSerialDev_Write(&slave, &msg, COS_BLOCK_FOREVER);
	pthread_join(thread_handle, &status);
	ASSERT(err == CCSOFTSERIAL_OK, "Got error writing: %d", err);

	/* Repeat above, but target is a master, not a slave.
	 */
	CCSoftSerialDev_Select(&master[0], CCSoftSerialDev_GetID(&master[1]), COS_BLOCK_FOREVER);
	thread_err = pthread_create(&thread_handle, &attr, data_transfer_read, &master[1]);
	if( thread_err != 0 ) {
		pthread_join(thread_handle, &status);
		ABORT_TEST("Error creating thread");
	}

	err = CCSoftSerialDev_Write(&master[0], &msg, COS_BLOCK_FOREVER);
	pthread_join(thread_handle, &status);
	ASSERT(err == CCSOFTSERIAL_OK, "Got error writing: %d", err);

	/* Slave writes and master reads.
	 */
	thread_err = pthread_create(&thread_handle, &attr, data_transfer_read, &master[0]);
	if( thread_err != 0 ) {
		pthread_join(thread_handle, &status);
		ABORT_TEST("Error creating thread");
	}
	
	err = CCSoftSerialDev_Write(&master[1], &msg, COS_BLOCK_FOREVER);
	pthread_join(thread_handle, &status);
	ASSERT(err == CCSOFTSERIAL_OK, "Got error writing: %d", err);
}

TEST_SUITE(soft_serial)
{
	ADD_TEST(arbitration_max_masters);
	ADD_TEST(arbitration_priority);
	ADD_TEST(arbitration_reselection);
	ADD_TEST(arbitration_timeout);
	ADD_TEST(bus_release);
	ADD_TEST(is_selected_current_master);
	ADD_TEST(is_selected_current_slave);
	ADD_TEST(is_selected_slave_timeout);
	ADD_TEST(is_selected_blocking_slave);
	ADD_TEST(data_transfer);
}
