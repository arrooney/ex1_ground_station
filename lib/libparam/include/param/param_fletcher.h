#ifndef PARAM_PARAM_FLETCHER_H_
#define PARAM_PARAM_FLETCHER_H_

uint16_t fletcher16_memcpy(uint8_t *data, int count, void * (*memcpyfcn)(void *, const void *, size_t));

#ifdef __AVR__
#include <avr/pgmspace.h>
static inline uint16_t fletcher16_P(uint8_t* data, int count) {
	uint16_t sum1 = 0;
	uint16_t sum2 = 0;
	int index;
	for (index = 0; index < count; ++index) {
		sum1 = (sum1 + pgm_read_byte(&data[index])) % 255;
		sum2 = (sum2 + sum1) % 255;
	}
	return (sum2 << 8) | sum1;
}
#endif

static inline uint16_t fletcher16(uint8_t* data, int count) {
	return fletcher16_memcpy(data, count, NULL);
}

#endif /* PARAM_FLETCHER_H_ */
