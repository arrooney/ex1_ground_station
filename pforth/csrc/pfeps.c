#include <pfeps.h>
#include <nanopower2.h>
#include <pfgomshell.h>

#define INDEX_BOUND(index, bound) (index) > (bound) ? (bound) : (index)

/* 
 * This global holds the current eps hk request.
 * It's been pre-initialized for testing purposes.
 */
static eps_hk_t eps_hk = { .vboost = {0, 1, 2},
			   .vbatt = 3,
			   .curin = {4, 5, 6},
			   .cursun = 7,
			   .cursys = 8,
			   .reserved = 9,
			   .curout = {10, 11, 12, 13, 14 ,15},
			   .output = {16, 17, 18, 19, 20, 21, 22, 23},
			   .output_on_delta = {24, 25, 26, 27, 28, 29, 30, 31},
			   .output_off_delta = {32, 33, 34, 35, 36,37, 38, 39},
			   .latchup = {40, 41, 42, 43, 44, 45},
			   .wdt_i2c_time_left = 46,
			   .wdt_gnd_time_left = 47,
			   .wdt_csp_pings_left = {48, 49},
			   .counter_wdt_i2c = 50,
			   .counter_wdt_gnd = 51,
			   .counter_wdt_csp = {52, 53},
			   .counter_boot = 54,
			   .temp = {55, 56, 57, 58, 59, 60},
			   .bootcause = 61,
			   .battmode = 62,
			   .pptmode = 63,
			   .reserved2 = 64
};

void epsUpdateHK( )
{
	int err;
	
	/* update eps_hk 
	 */
	err = eps_hk_get(&eps_hk);
	if( err != sizeof(eps_hk) ) {
		PUSH_DATA_STACK(GOMSHELL_ERR_COM);
	}
	else {
		PUSH_DATA_STACK(GOMSHELL_OK);
	}
}

/*
 * Several magic numbers in this function. They are
 * all derived from the eps_hk_t structure defined in
 * nanopower2.h.
 */
void epsIndexHK( cell_t struct_index, cell_t array_index )
{
	int64_t hk_value;
	
	switch(struct_index) {
	case 1:
		/* vboost[3] */
		array_index = INDEX_BOUND(array_index, 3);
		hk_value = (int64_t) eps_hk.vboost[array_index];
		break;
	case 2:
		/* vbatt */
		hk_value = (int64_t) eps_hk.vbatt;
		break;
	case 3:
		/* curin[3] */
		array_index = INDEX_BOUND(array_index, 3);
		hk_value = (int64_t) eps_hk.curin[array_index];
		break;
	case 4:
		/* cursun */
		hk_value = (int64_t) eps_hk.cursun;
		break;
	case 5:
		/* cursys */
		hk_value = (int64_t) eps_hk.cursys;
		break;
	case 7:
		/* curout[6] */
		array_index = INDEX_BOUND(array_index, 6);
		hk_value = (int64_t) eps_hk.curout[array_index];
		break;
	case 8:
		/* output[8] */
		array_index = INDEX_BOUND(array_index, 8);
		hk_value = (int64_t) eps_hk.output[array_index];
		break;
	case 9:
		/* output_on_delta[8] */
		array_index = INDEX_BOUND(array_index, 8);
		hk_value = (int64_t) eps_hk.output_on_delta[array_index];
		break;
	case 10:
		/* output_off_delta[8] */
		array_index = INDEX_BOUND(array_index, 8);
		hk_value = (int64_t) eps_hk.output_off_delta[array_index];
		break;
	case 11:
		/* latchup[6] */
		array_index = INDEX_BOUND(array_index, 6);
		hk_value = (int64_t) eps_hk.latchup[array_index];
		break;
	case 12:
		/* wdt_i2c_time_left */
		hk_value = (int64_t) eps_hk.wdt_i2c_time_left;
		break;
	case 13:
		/* wdt_gnd_time_left */
		hk_value = (int64_t) eps_hk.wdt_gnd_time_left;
		break;
	case 14:
		/* wdt_csp_pings_left[2] */
		array_index = INDEX_BOUND(array_index, 2);
		hk_value = (int64_t) eps_hk.wdt_csp_pings_left[array_index];
		break;
	case 15:
		/* counter_wdt_i2c */
		hk_value = (int64_t) eps_hk.counter_wdt_i2c;
		break;
	case 16:
		/* counter_wdt_gnd */
		hk_value = (int64_t) eps_hk.counter_wdt_gnd;
		break;
	case 17:
		/* counter_wdt_csp[2] */
		array_index = INDEX_BOUND(array_index, 2);
		hk_value = (int64_t) eps_hk.counter_wdt_csp[array_index];
		break;
	case 18:
		/* counter_boot */
		hk_value = (int64_t) eps_hk.counter_boot;
		break;
	case 19:
		/* temp[6] */
		array_index = INDEX_BOUND(array_index, 6);
		hk_value = (int64_t) eps_hk.temp[array_index];		
		break;
	case 20:
		/* bootcause */
		hk_value = (int64_t) eps_hk.bootcause;
		break;
	case 21:
		/* battmode */
		hk_value = (int64_t) eps_hk.battmode;
		break;
	case 22:
		/* pptmode */
		hk_value = (int64_t) eps_hk.pptmode;
		break;
	default:
		hk_value = -1;
		break;
	}

	if( hk_value > INT32_MAX ) {
		sdTerminalPrint("HK Request overflow\n");
	}
	if( hk_value < INT32_MIN ) {
		sdTerminalPrint("HK Request underflow\n");
	}
	PUSH_DATA_STACK(((cell_t) hk_value));
}

void epsPrint( void )
{
	eps_hk_print(&eps_hk);
}
