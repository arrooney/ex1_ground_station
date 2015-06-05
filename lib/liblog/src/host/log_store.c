/**
 * @name Log store
 *
 * The data is stored in a virtual memory area using util/vmem.h
 * Here is a figure of the memory area:
 *
 * +-------+------------------------------------+
 * | Index |                Data                |
 * +-------+------------------------------------+
 *
 * The first 4 or 8 bytes are used to store the ring-buffer index
 * The data field is a ring-buffer where each element is stored
 *
 * +-----+------------------+-----+
 * | Len |     Element      | Len |
 * +-----+------------------+-----+
 *
 * Each packed element is stored with a length field in front and
 * after. This makes it easy to skip either forwards or backwards in
 * the data. It also acts as a sanity check to the parser to help
 * detect corruption in the data.
 *
 */

#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <log/log.h>
#include <util/timestamp.h>
#include <util/hexdump.h>
#include <util/vmem.h>

/**
 * The input pointer is a stored pointer in memory that gives the current
 * ring-buffer input address
 */
static vmemptr_t input_ptr_addr = VMEM_NULL;

/**
 * Begin- and end addresses with length field defines buffer space
 * These must be initialized by the log_store_init call
 */
static vmemptr_t begin = VMEM_NULL;
static vmemptr_t end = VMEM_NULL;
static int bufferlen = 0;
static int log_inited = 0;

/**
 * We store a local copy of the input_ptr value in RAM
 * so we don't have to read it for each time log_store is called
 */
static vmemptr_t input_ptr = VMEM_NULL;

void log_store_init(vmemptr_t start_addr, int length) {

	if (length < 512)
		return;

	/* Store settings */
	input_ptr_addr = start_addr;
	begin = start_addr + sizeof(vmemptr_t);
	end = start_addr + length;
	bufferlen = length - sizeof(vmemptr_t);

	/* Read input ptr and validate it */
	vmemptr_t ptr = VMEM_NULL;
	VMEM_CPY((uintptr_t) &ptr, input_ptr_addr, sizeof(vmemptr_t));
	if (ptr < begin || ptr > end) {
		VMEM_CPY(input_ptr_addr, (uintptr_t) &begin, sizeof(vmemptr_t));
	}

	log_inited = 1;

}

void log_store_foreach(int (*fct)(char * data, unsigned int size)) {

	if (log_inited != 1)
		return;

	uint8_t size8 = 0;
	vmemptr_t output_ptr = VMEM_NULL;
	int count = 0;

	/* Read input ptr */
	VMEM_CPY((uintptr_t) &output_ptr, input_ptr_addr, sizeof(vmemptr_t));

	/* Check pointer */
	if (output_ptr == VMEM_NULL || output_ptr < begin || output_ptr > end)
		return;

	while(1) {

		/* Decrement output_ptr by 1 */
		if (output_ptr == begin)
			output_ptr = end - 1;
		else
			output_ptr--;

		/* Read size of prev element */
		VMEM_CPY((uintptr_t) &size8, output_ptr, 1);
		if (size8 == 0)
			break;

		/* Decrement ptr */
		if (output_ptr - size8 < begin)
			output_ptr = (output_ptr + bufferlen) - size8;
		else
			output_ptr -= size8;

		/* Decrement output_ptr by 1 */
		if (output_ptr == begin)
			output_ptr = end - 1;
		else
			output_ptr--;

		/* Validate against buffer turnaround */
		count += size8 + 2;
		if (count >= bufferlen) {
			//printf("Turnaround\r\n");
			break;
		}

		/* Validate length */
		uint8_t size8_check = 0;
		VMEM_CPY((uintptr_t) &size8_check, output_ptr, 1);
		if (size8_check != size8) {
			//printf("Invalid length %u %u\r\n", size8_check, size8);
			break;
		}

		output_ptr++;
		if (output_ptr == end)
			output_ptr = begin;

		/* Read out data onto stack */
		char out[size8+1];
		if (output_ptr + size8 > end) {
			int second = (output_ptr + size8) - end;
			int first = size8 - second;
			VMEM_CPY((uintptr_t) out, output_ptr, first);
			VMEM_CPY((uintptr_t) out + first, begin, second);
		} else {
			VMEM_CPY((uintptr_t) out, output_ptr, size8);
		}
		out[size8] = '\0';

		/* Callback with data */
		if ((*fct)(out, size8) < 0)
			return;

		/* Decrement output_ptr by 1 */
		if (output_ptr == begin)
			output_ptr = end - 1;
		else
			output_ptr--;

	}

}

void log_store(char * data, unsigned int size) {

	if (log_inited != 1)
		return;

	if (size > UINT8_MAX)
		return;

	/* Cast size to uint8 */
	uint8_t size8 = (uint8_t) size;

	/* Read next ptr from memory */
	if (input_ptr == VMEM_NULL)
		VMEM_CPY((uintptr_t) &input_ptr, input_ptr_addr, sizeof(vmemptr_t));

	/* Check pointer */
	if (input_ptr == VMEM_NULL || input_ptr < begin || input_ptr > end)
		return;

	/* Write size */
	VMEM_CPY(input_ptr, (uintptr_t) &size8, sizeof(size8));
	input_ptr += sizeof(size8);
	if (input_ptr >= end)
		input_ptr = begin;

	/* Write data */
	if (input_ptr + size > end) {
		int first = end - input_ptr;
		int second = size - first;
		VMEM_CPY(input_ptr, (uintptr_t) data, first);
		input_ptr = begin;
		VMEM_CPY(input_ptr, (uintptr_t) data + first, second);
		input_ptr += second;
	} else {
		VMEM_CPY(input_ptr, (uintptr_t) data, size);
		input_ptr += size;
		if (input_ptr >= end)
			input_ptr = begin;
	}

	/* Write size */
	VMEM_CPY(input_ptr, (uintptr_t) &size8, sizeof(size8));
	input_ptr += sizeof(size8);
	if (input_ptr >= end)
		input_ptr = begin;

	/* Write next ptr */
	VMEM_CPY(input_ptr_addr, (uintptr_t) &input_ptr, sizeof(vmemptr_t));

}
