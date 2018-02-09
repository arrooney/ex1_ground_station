#ifndef PF_DFGM_WORDS
#define PF_DFGM_WORDS

#include "pf_all.h"

#define DFGM_MOCK_PACKET_SIZE 1248
#define SECONDS_OF_MIN_MAX 250

void dfgm_s16tobe( int16_t val, uint8_t* ar );
int16_t dfgm_betos16( uint8_t* ar );
void dfgm_u32tobe( uint32_t val, uint8_t* ar );
uint32_t dfgm_betou32( uint8_t* ar );
void dfgm_mock_packet_value( void );
void dfgm_mock_min_max_toggle( void );
void dfgm_mock_use_ring( cell_t start_, cell_t count_, cell_t data_ );
void dfgm_mock_init( cell_t dev_, cell_t dev_len_ );

#endif /*PF_DFGM_WORDS*/
