#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint16_t fletcher16_memcpy(uint8_t *data, int count, void * (*memcpyfcn)(void *, const void *, size_t)) {
	if (memcpyfcn == NULL)
		memcpyfcn = &memcpy;

	uint16_t sum1 = 0;
	uint16_t sum2 = 0;
	int index;
	for (index = 0; index < count; ++index) {
		uint8_t byte;
		(*memcpyfcn)(&byte, &data[index], 1);
		sum1 = (sum1 + byte) % 255;
		sum2 = (sum2 + sum1) % 255;
	}
	return (sum2 << 8) | sum1;
}

