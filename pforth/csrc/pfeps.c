#include <pfeps.h>
#include <nanopower2.h>

static eps_hk_t eps_hk;

void epsUpdateHK( )
{
	/* update eps_hk */
}

void epsIndexHK( cell_t struct_index, cell_t array_index )
{
	switch(struct_index) {
	case 1:
		/* vboost[3] */
		break;
	case 2:
		/* vbatt */
		break;
	case 3:
		/* curin[3] */
		break;
	case 4:
		/* cursun */
		break;
	}
}
