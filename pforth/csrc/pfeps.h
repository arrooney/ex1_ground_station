#ifndef PF_EPS_WORDS
#define PF_EPS_WORDS

#include "pf_all.h"

void epsUpdateHK( );
void epsCheckVBatt( cell_t min, cell_t max );
void epsCheckTemp( cell_t node, cell_t min, cell_t max );
void epsCheckWDT( cell_t wd );
void epsCheckLatchUps( cell_t channel, cell_t max );

#endif
