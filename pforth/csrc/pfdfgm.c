#ifdef GOMSHELL

#include "pfdfgm.h"
#include "pfrings.h"
#include <serial.h>

#define DFGM_MOCK_PACKET_SIZE 1248
#define SECONDS_OF_MIN_MAX 250

static uint8_t dfgm_mock_packet[DFGM_MOCK_PACKET_SIZE] = {0};
static uint8_t* volatile dfgm_true_packet = NULL;
static size_t volatile dfgm_true_packet_length = 0;
static size_t volatile true_packet_index = 0;

/**
 * Convert signed 16 bit integer val into network order byte array.
 */
void dfgm_s16tobe( int16_t val, uint8_t* ar )
{
	ar[0] = (uint8_t) (((val & 0xFF00) >> 8) & 0x00FF);
	ar[1] = (uint8_t) (val & 0x00FF);
}

/** 
 * Convert network order byte array into signed 16 bit integer.
 */
int16_t dfgm_betos16( uint8_t* ar )
{
	int16_t s = (int16_t) ((((uint16_t) ar[0]) << 8)
			       + ((uint16_t) ar[1]));
	return s;
}

/**
 * Convert usigned 32 bit integer val into network order byte array
 */
void dfgm_u32tobe( uint32_t val, uint8_t* ar )
{
	ar[0] = (uint8_t) ((val & 0xFF000000) >> 24);
	ar[1] = (uint8_t) ((val & 0x00FF0000) >> 16);
	ar[2] = (uint8_t) ((val & 0x0000FF00) >> 8);
	ar[3] = (uint8_t) (val & 0x000000FF);
}

/**
 * Convert network order byte array into 32 bit unsigned integer.
 */
uint32_t dfgm_betou32( uint8_t* ar )
{
	uint32_t s = (((uint32_t) ar[0]) << 24)
		+ (((uint32_t) ar[1]) << 16)
		+ (((uint32_t) ar[2]) << 8)
		+ ((uint32_t) ar[3]);
	return s;
}

/**
 * Print sample point in current DFGM packet.
 */
void dfgm_mock_packet_value( void )
{
	int16_t dac = dfgm_betos16(&dfgm_mock_packet[36]);
	int16_t adc = dfgm_betos16(&dfgm_mock_packet[38]);
	sdTerminalPrint("pid: %d\ndac: %d, adc: %d, sum: %d\nread index: %zu\n",
			dfgm_mock_packet[2], dac, adc, dac+adc, true_packet_index);
}

/**
 * This is a thread that performs the work to imitate the DFGM.
 * It writes to the serial port initialized by dfgm_mock_init().
 */
static void* dfgm_mock_thread_function( void* arg )
{
	struct serial_dev_t* dev = arg;
	size_t i;
	
	dfgm_mock_packet[0] = 0x10;
	dfgm_mock_packet[1] = 0x02;
	dfgm_mock_packet[2] = 0;
	dfgm_mock_packet[1245] = 0x03;
	sdTerminalPrint("Mock start");
	sdTerminalFlush();

	for( i = 0; i < 100; ++i ) {
		dfgm_s16tobe(-100, &dfgm_mock_packet[36+12*i]);
		dfgm_s16tobe(-300, &dfgm_mock_packet[36+12*i+2]);
		dfgm_s16tobe(-100, &dfgm_mock_packet[36+12*i+4]);
		dfgm_s16tobe(-300, &dfgm_mock_packet[36+12*i+6]);
		dfgm_s16tobe(-100, &dfgm_mock_packet[36+12*i+8]);
		dfgm_s16tobe(-300, &dfgm_mock_packet[36+12*i+10]);
	}
	
	sdTerminalPrint("Mock running\n");
	sdTerminalFlush();
	for( ;; ) {
		if( dfgm_true_packet != NULL ) {
			if( serial_putstr(dev, (char*) dfgm_true_packet + true_packet_index, 1248) != 1248 ) {
				sdTerminalPrint("dfgm mock: error writing to uart\n");
				sdTerminalFlush();
			}
			true_packet_index += 1248;
			if( true_packet_index >= dfgm_true_packet_length ) {
				free(dfgm_true_packet);
				dfgm_true_packet = NULL;
				true_packet_index = 0;
				sdTerminalPrint("DFGM Mock: Packets read all transmitted");
				sdTerminalFlush();
				sleep(5);
			}
			sleep(1);
			continue;
		}

		if( serial_putstr(dev, (char*) dfgm_mock_packet, 1248) != 1248 ) {
			sdTerminalPrint("dfgm mock: error writing to uart\n");
			sdTerminalFlush();
		}
		++dfgm_mock_packet[2];
		for( i = 0; i < 100; ++i ) {
			int16_t xdac = dfgm_betos16(&dfgm_mock_packet[36+12*i]);
			int16_t xadc = dfgm_betos16(&dfgm_mock_packet[36+12*i+2]);

			int16_t ydac = dfgm_betos16(&dfgm_mock_packet[36+12*i+4]);
			int16_t yadc = dfgm_betos16(&dfgm_mock_packet[36+12*i+6]);

			int16_t zdac = dfgm_betos16(&dfgm_mock_packet[36+12*i+8]);
			int16_t zadc = dfgm_betos16(&dfgm_mock_packet[36+12*i+10]);

			++xdac;
			++xadc;
			++ydac;
			++yadc;
			++zdac;
			++zadc;

			dfgm_s16tobe(xdac, &dfgm_mock_packet[36+12*i]);
			dfgm_s16tobe(xadc, &dfgm_mock_packet[36+12*i+2]);

			dfgm_s16tobe(ydac, &dfgm_mock_packet[36+12*i+4]);
			dfgm_s16tobe(yadc, &dfgm_mock_packet[36+12*i+6]);

			dfgm_s16tobe(zdac, &dfgm_mock_packet[36+12*i+8]);
			dfgm_s16tobe(zadc, &dfgm_mock_packet[36+12*i+10]);
		}
		sleep(1);
	}

	return NULL;
}

/**
 * Initialize a several seconds of data intended to cause over flow
 * in nanominds DSP of DFGM data.
 * This data will automatically be written to USART if the dfgm thread
 * is running.
 */
void dfgm_mock_min_max_toggle( void )
{
	size_t i, j;
	uint8_t pid = 0;
	uint8_t* temp_dfgm_true_packet;
	
	if( dfgm_true_packet != NULL ) {
		sdTerminalPrint("DFGM Mock: previous packets still being transmitted");
		sdTerminalFlush();
		return;
	}

	temp_dfgm_true_packet = malloc(sizeof(uint8_t) * DFGM_MOCK_PACKET_SIZE * SECONDS_OF_MIN_MAX * 2);
	if( temp_dfgm_true_packet == NULL ) {
		sdTerminalPrint("DFGM Mock: Failed to allocate memory");
		sdTerminalFlush();
		return;
	}

	/* Set max. */
	for( i = 0; i < SECONDS_OF_MIN_MAX; ++i ) {
		/* Set packet header/tails
		 */
		temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)] = 0x10;
		temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+1] = 0x02;
		temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+2] = pid;
		temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+1245] = 0x03;

		for( j = 0; j < 100; ++j ) {
			dfgm_s16tobe(INT16_MAX, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j)]);
			dfgm_s16tobe(INT16_MIN, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j+2)]);
			
			dfgm_s16tobe(INT16_MAX, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j+4)]);
			dfgm_s16tobe(INT16_MIN, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j+6)]);

			dfgm_s16tobe(INT16_MAX, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j+8)]);
			dfgm_s16tobe(INT16_MIN, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j+10)]);
		}

		pid = (pid+1)%256;
	}


	/* Set min */
	for( i = SECONDS_OF_MIN_MAX; i < SECONDS_OF_MIN_MAX*2; ++i ) {
		/* Set packet header/tails
		 */
		temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)] = 0x10;
		temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+1] = 0x02;
		temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+2] = pid;
		temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+1245] = 0x03;

		/* Set min max data.
		 */
		for( j = 0; j < 100; ++j ) {
			dfgm_s16tobe(INT16_MIN, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j)]);
			dfgm_s16tobe(INT16_MAX, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j+2)]);
			
			dfgm_s16tobe(INT16_MIN, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j+4)]);
			dfgm_s16tobe(INT16_MAX, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j+6)]);

			dfgm_s16tobe(INT16_MIN, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j+8)]);
			dfgm_s16tobe(INT16_MAX, &temp_dfgm_true_packet[(i*DFGM_MOCK_PACKET_SIZE)+(36+12*j+10)]);
		}

		pid = (pid+1)%256;
	}

	sdTerminalPrint("DFGM mock: min (%" PRId16 ") max (%" PRId16 ") set\n", INT16_MIN, INT16_MAX);
	dfgm_true_packet_length = DFGM_MOCK_PACKET_SIZE * SECONDS_OF_MIN_MAX * 2;
	dfgm_true_packet = temp_dfgm_true_packet;

}

/**
 * Read in data from raw DFGM files downloaded from Ex-Alta 1. This data will
 * be automatically written to USARt if the the dfgm thread is running.
 */
void dfgm_mock_use_ring( cell_t start_, cell_t count_, cell_t data_ )
{
	size_t i;
	char * bucket_start = (char*) start_;
	char bucket_path[50];
	size_t total_buckets = (size_t) count_;
	size_t bytes_per_bucket = (size_t) data_;
	uint8_t* true_packet_pointer;
	uint8_t* temp_dfgm_true_packet;

	if( dfgm_true_packet != NULL ) {
		sdTerminalPrint("DFGM Mock: previous packets still being transmitted");
		return;
	}

	sdTerminalPrint("start: %p\nbuckets: %zu\ndata/bucket: %zu\n", bucket_start, total_buckets, bytes_per_bucket);

	temp_dfgm_true_packet = malloc(sizeof(uint8_t) * bytes_per_bucket * total_buckets);
	if( temp_dfgm_true_packet == NULL ) {
		sdTerminalPrint("DFGM Mock: Failed to allocate memory");
		sdTerminalFlush();
		return;
	}

	snprintf(bucket_path, 50, "%.*s\0", GOMSHELL_RING_NAME_LENGTH, bucket_start);
	true_packet_pointer = temp_dfgm_true_packet;
	for( i = 0; i < total_buckets; ++i ) {
		/* Open file
		 */
		FILE* file = fopen(bucket_path, "r");
		if( file == NULL ) {
			sdTerminalPrint("DFGM Mock: Failed to open %s\n", bucket_path);
			if( i == 0 ) {
				free(temp_dfgm_true_packet);
				temp_dfgm_true_packet = NULL;
			}
			break;
		}
		
		/* Read file
		 */
		fseek(file, sizeof(uint32_t), SEEK_SET); /* skip time stamp. */
		size_t bytes_read = fread(true_packet_pointer, sizeof(uint8_t), (bytes_per_bucket-4), file);
		if( bytes_read != (bytes_per_bucket-4) ) {
			sdTerminalPrint("DFGM Mock: Failed to read %zu bytes from %.*s", (bytes_per_bucket-4), GOMSHELL_RING_NAME_LENGTH, bucket_start);
			break;
		}
		
		/* Close file
		 */
		true_packet_pointer += (bytes_per_bucket-4);

		/* Go to next bucket name.
		 */
		pf_next_name(bucket_start);
		snprintf(bucket_path, 50, "%.*s\0", GOMSHELL_RING_NAME_LENGTH, bucket_start);
	}

	sdTerminalPrint("buckets read: %zu\n", i);
	dfgm_true_packet_length = i*(bytes_per_bucket-4);
	dfgm_true_packet = temp_dfgm_true_packet;
}

/**
 * Initialize USART that DFGM thread will write to and then start the thread.
 */
void dfgm_mock_init( cell_t dev_, cell_t dev_len_ )
{
	const char* dev;
	char* dev_str;
	size_t dev_length;
	
	static int is_init = 0;
	static struct serial_dev_t serial_dev;
	static pthread_t dfgm_mock_thread;

	if( is_init == 1 ) {
		sdTerminalPrint("DFGM Mock initialized");
		return;
	}

	dev = (const char*) dev_;
	dev_length = (size_t) dev_len_;

	dev_str = malloc(sizeof(char) * dev_length + 1);
	if( dev_str == NULL ) {
			sdTerminalPrint("Failed to malloc");
	}
	strncpy(dev_str, dev, dev_length);
	dev_str[dev_length] = '\0';

	if( serial_dev_init(&serial_dev, dev_str, 115200) != 0 ) {
		sdTerminalPrint("Try again");
		return;
	}
	pthread_create(&dfgm_mock_thread, NULL, dfgm_mock_thread_function, &serial_dev);
	is_init = 1;
}

#endif
