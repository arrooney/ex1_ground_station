/*
 *
 *
 */

#include <string.h>
#include <inttypes.h>

#include <nanohub.h>

#include <util/error.h>
#include <util/log.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>

uint8_t node_hub = NANOHUB_CSP_ADDR;

int hub_set_node(uint8_t node) {
	node_hub = node;
	return 1;
}

int hub_get_hk(nanohub_hk_t *nanohub_hk) {

	int status = csp_transaction(CSP_PRIO_NORM, node_hub, NANOHUB_PORT_HK, 1000, NULL, 0, nanohub_hk, sizeof(nanohub_hk_t));
	if (status != sizeof(nanohub_hk_t))
		return -1;

    float cur_gain[3] = {0.391/0.8, 0.388/0.8, 0.389/0.8};
    float cur_offset[3] = {-2.409, -1.192, 2.913};
    float volt_gain[2] = {1.988, 2.327};
    float volt_offset[2] = {2.890, -4.400};

	nanohub_hk->bootcount = csp_ntoh32(nanohub_hk->bootcount);
	nanohub_hk->temp = csp_ntoh16(nanohub_hk->temp);
	nanohub_hk->Iout[0] = csp_ntoh16(nanohub_hk->Iout[0]);
	nanohub_hk->Iout[1] = csp_ntoh16(nanohub_hk->Iout[1]);
	nanohub_hk->Iout[2] = csp_ntoh16(nanohub_hk->Iout[2]);
	nanohub_hk->Vout[0] = csp_ntoh16(nanohub_hk->Vout[0]);
	nanohub_hk->Vout[1] = csp_ntoh16(nanohub_hk->Vout[1]);

    // Fixing uncalibratable nanohub here.

    nanohub_hk->Iout[0] = nanohub_hk->Iout[0]*cur_gain[0] + cur_offset[0];
    nanohub_hk->Iout[1] = nanohub_hk->Iout[1]*cur_gain[1] + cur_offset[1];
    nanohub_hk->Iout[2] = nanohub_hk->Iout[2]*cur_gain[2] + cur_offset[2];
    nanohub_hk->Vout[0] = nanohub_hk->Vout[0]*volt_gain[0] + volt_offset[0];
    nanohub_hk->Vout[1] = nanohub_hk->Vout[1]*volt_gain[1] + volt_offset[1];
	return status;
}

int hub_conf_cmd(uint8_t cmd, uint8_t *configmode)
{
	uint8_t data[2];
	data[0] = cmd;
	data[1] = 0xC8; // Magic byte

	int status = csp_transaction(CSP_PRIO_NORM, node_hub, NANOHUB_PORT_CONF_CMD, 1000, data, 2, configmode , 1);

	return status;
}

void hub_print_conf(nanohub_conf_t *configuration)
{
	printf("Configuration is : \r\n");

	printf("Knife 0 : \r\n");
	printf(" time         %u %u\r\n",configuration->knife0.time[0],configuration->knife0.time[1]);
	printf(" retries      %u %u\r\n",configuration->knife0.retries[0],configuration->knife0.retries[1]);
	printf(" bootdelay    %"PRIu32" %"PRIu32"\r\n",configuration->knife0.bootdelay[0],configuration->knife0.bootdelay[1]);
	printf(" delay        %u %u\r\n",configuration->knife0.delay[0],configuration->knife0.delay[1]);
	printf(" enable       %u %u\r\n",configuration->knife0.enable[0],configuration->knife0.enable[1]);
	printf(" sensemask    0x%x 0x%x\r\n",configuration->knife0.sensemask[0],configuration->knife0.sensemask[1]);
	printf(" sense_no     %u\r\n",configuration->knife0.sense_no);

	printf("Knife 1 : \r\n");
	printf(" time         %u %u\r\n",configuration->knife1.time[0],configuration->knife1.time[1]);
	printf(" retries      %u %u\r\n",configuration->knife1.retries[0],configuration->knife1.retries[1]);
	printf(" bootdelay    %"PRIu32" %"PRIu32"\r\n",configuration->knife1.bootdelay[0],configuration->knife1.bootdelay[1]);
	printf(" delay        %u %u\r\n",configuration->knife1.delay[0],configuration->knife1.delay[1]);
	printf(" enable       %u %u\r\n",configuration->knife1.enable[0],configuration->knife1.enable[1]);
	printf(" sensemask    0x%x 0x%x\r\n",configuration->knife1.sensemask[0],configuration->knife1.sensemask[1]);
	printf(" sense_no     %u\r\n",configuration->knife1.sense_no);

	printf("Switch 0 : \r\n");
	printf(" bootdelay    %u \r\n",configuration->switch0.bootdelay);
	printf("Switch 1 : \r\n");
	printf(" bootdelay    %u \r\n",configuration->switch1.bootdelay);
    printf("Switch 2: \r\n");
    printf(" bootdelay    %u \r\n",configuration->switch2.bootdelay);

	printf("Timing : \r\n");
	printf(" wdt timeout  %"PRIu32" \r\n",configuration->timing.wdt_timeout);
	printf(" wdt enable   %"PRIu8" \r\n",configuration->timing.wdt_enable);
	printf(" com tx inhibit timeout      %"PRIu32" \r\n",configuration->timing.com_tx_inhibit_timeout);
	printf(" com tx inhibit enable       %"PRIu8" \r\n",configuration->timing.com_tx_inhibit_enable);

	printf("DIO : \r\n");
	printf(" enable       %X \r\n",configuration->dio.enable);
	printf(" dirmask      %X \r\n",configuration->dio.dirmask);
	printf(" outputvalue  %X \r\n",configuration->dio.outputvalue);

}


void hub_edit_conf(nanohub_conf_t *configuration)
{
	unsigned int i;
	uint32_t longi;
	int result __attribute__((__unused__));

	printf("Edit nanohub configuration  \r\n -------------------------- \r\n");

	printf("Knife 0 (press 'e' to edit) \r\n");
	if (getchar()=='e')
	{
		printf("Knife 0 channel 0: (press 'e' to edit parameters) \n\r");
		printf(" time         %u\r\n",configuration->knife0.time[0]);
		if (getchar() == 'e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife0.time[0] = (uint16_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" retries      %u\r\n",configuration->knife0.retries[0]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife0.retries[0] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" bootdelay    %"PRIu32"\r\n",configuration->knife0.bootdelay[0]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%"PRIu32"",&longi);
			configuration->knife0.bootdelay[0] = (uint32_t) longi;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" delay        %u\r\n",configuration->knife0.delay[0]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife0.delay[0] = (uint16_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" enable       %u\r\n",configuration->knife0.enable[0]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife0.enable[0] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}
		printf(" sensemask    0x%x\r\n",configuration->knife0.sensemask[0]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%x",&i);
			configuration->knife0.sensemask[0] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf("Knife 0 channel 1: (press 'e' to edit parameters) \n\r");
		printf(" time         %u\r\n",configuration->knife0.time[1]);
		if (getchar() == 'e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife0.time[1] = (uint16_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" retries      %u\r\n",configuration->knife0.retries[1]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife0.retries[1] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" bootdelay    %"PRIu32"\r\n",configuration->knife0.bootdelay[1]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%"PRIu32"",&longi);
			configuration->knife0.bootdelay[1] = (uint32_t) longi;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" delay        %u\r\n",configuration->knife0.delay[1]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife0.delay[1] = (uint16_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" enable       %u\r\n",configuration->knife0.enable[1]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife0.enable[1] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}
		printf(" sensemask    0x%x\r\n",configuration->knife0.sensemask[1]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%x",&i);
			configuration->knife0.sensemask[1] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}



		printf(" sense_no     %u\r\n",configuration->knife0.sense_no);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%x",&i);
			configuration->knife0.sense_no = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}

	}

	printf("Knife 1 (press 'e' to edit) \r\n");
	if (getchar()=='e')
	{
		printf("Knife 0 channel 0: (press 'e' to edit parameters) \n\r");
		printf(" time         %u\r\n",configuration->knife1.time[0]);
		if (getchar() == 'e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife1.time[0] = (uint16_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" retries      %u\r\n",configuration->knife1.retries[0]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife1.retries[0] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" bootdelay    %"PRIu32"\r\n",configuration->knife1.bootdelay[0]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%"PRIu32"",&longi);
			configuration->knife1.bootdelay[0] = (uint32_t) longi;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" delay        %u\r\n",configuration->knife1.delay[0]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife1.delay[0] = (uint16_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" enable       %u\r\n",configuration->knife1.enable[0]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife1.enable[0] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}
		printf(" sensemask    0x%x\r\n",configuration->knife1.sensemask[0]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%x",&i);
			configuration->knife1.sensemask[0] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf("Knife 0 channel 1: (press 'e' to edit parameters) \n\r");
		printf(" time         %u\r\n",configuration->knife1.time[1]);
		if (getchar() == 'e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife1.time[1] = (uint16_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" retries      %u\r\n",configuration->knife1.retries[1]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife1.retries[1] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" bootdelay    %"PRIu32"\r\n",configuration->knife1.bootdelay[1]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%"PRIu32"",&longi);
			configuration->knife1.bootdelay[1] = (uint32_t) longi;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" delay        %u\r\n",configuration->knife1.delay[1]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife1.delay[1] = (uint16_t) i;
			fflush(stdin);
			printf("\r\n");
		}

		printf(" enable       %u\r\n",configuration->knife1.enable[1]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife1.enable[1] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}
		printf(" sensemask    0x%x\r\n",configuration->knife1.sensemask[1]);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%x",&i);
			configuration->knife1.sensemask[1] = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}



		printf(" sense_no     %u\r\n",configuration->knife1.sense_no);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->knife1.sense_no = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}

	}



	printf("Switch 0 : (press 'e' to edit)\r\n");
	if (getchar()=='e')
	{
		printf(" bootdelay    %u \r\n",configuration->switch0.bootdelay);
		printf(">");
		result = scanf("%u",&i);
		configuration->switch0.bootdelay = (uint16_t) i;
		fflush(stdin);
		printf("\r\n");
	}
	printf("Switch 1 : (press 'e' to edit)\r\n");
	if (getchar()=='e')
	{
		printf(" bootdelay    %u \r\n",configuration->switch1.bootdelay);
		printf(">");
		result = scanf("%u",&i);
		configuration->switch1.bootdelay = (uint16_t) i;
		fflush(stdin);
		printf("\r\n");
	}
	printf("Switch 2 : (press 'e' to edit)\r\n");
	if (getchar()=='e')
	{
		printf(" bootdelay    %u \r\n",configuration->switch2.bootdelay);
		printf(">");
		result = scanf("%u",&i);
		configuration->switch2.bootdelay = (uint16_t) i;
		fflush(stdin);
		printf("\r\n");
	}

	printf("Timing : (press 'e' to edit) \r\n");
	if (getchar()=='e')
	{
		printf(" wdt timeout  %"PRIu32" \r\n",configuration->timing.wdt_timeout);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->timing.wdt_timeout = (uint32_t) i;
			fflush(stdin);
			printf("\r\n");
		}
		printf(" wdt enable   %u \r\n",configuration->timing.wdt_enable);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->timing.wdt_enable = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}
		printf(" com tx inhibit timeout      %"PRIu32" \r\n",configuration->timing.com_tx_inhibit_timeout);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->timing.com_tx_inhibit_timeout = (uint16_t) i;
			fflush(stdin);
			printf("\r\n");
		}
		printf(" com tx inhibit enable       %u \r\n",configuration->timing.com_tx_inhibit_enable);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%u",&i);
			configuration->timing.com_tx_inhibit_enable = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}
	}

	printf("DIO :  (press 'e' to edit)\r\n");
	if (getchar()=='e')
	{
		printf(" enable (hex)     %X \r\n",configuration->dio.enable);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%x",&i);
			configuration->dio.enable = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}
		printf(" dirmask (hex)     %X \r\n",configuration->dio.dirmask);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%x",&i);
			configuration->dio.dirmask = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}
		printf(" outputvalue (hex) %X \r\n",configuration->dio.outputvalue);
		if (getchar()=='e')
		{
			printf(">");
			result = scanf("%x",&i);
			configuration->dio.outputvalue = (uint8_t) i;
			fflush(stdin);
			printf("\r\n");
		}
	}
}


int hub_get_conf(nanohub_conf_t *conf) {

	int status = csp_transaction(CSP_PRIO_NORM, node_hub, NANOHUB_PORT_CONF, 1000, NULL, 0, conf, sizeof(nanohub_conf_t));

	 conf->knife0.time[0] = csp_ntoh16(conf->knife0.time[0]);
	  conf->knife0.time[1] = csp_ntoh16(conf->knife0.time[1]);
	  conf->knife0.bootdelay[0] = csp_ntoh32(conf->knife0.bootdelay[0]);
	  conf->knife0.bootdelay[1] = csp_ntoh32(conf->knife0.bootdelay[1]);
	  conf->knife0.delay[0] = csp_ntoh16(conf->knife0.delay[0]);
	  conf->knife0.delay[1] = csp_ntoh16(conf->knife0.delay[1]);
	  conf->knife0.enable[0] = conf->knife0.enable[0];
	  conf->knife0.enable[1] = conf->knife0.enable[1];

	  conf->knife1.time[0] = csp_ntoh16(conf->knife1.time[0]);
	  conf->knife1.time[1] = csp_ntoh16(conf->knife1.time[1]);
	  conf->knife1.bootdelay[0] = csp_ntoh32(conf->knife1.bootdelay[0]);
	  conf->knife1.bootdelay[1] = csp_ntoh32(conf->knife1.bootdelay[1]);
	  conf->knife1.delay[0] = csp_ntoh16(conf->knife1.delay[0]);
	  conf->knife1.delay[1] = csp_ntoh16(conf->knife1.delay[1]);
	  conf->knife1.enable[0] = conf->knife1.enable[0];
	  conf->knife1.enable[1] = conf->knife1.enable[1];

	  conf->switch0.bootdelay = csp_ntoh16(conf->switch0.bootdelay);
	  conf->switch1.bootdelay = csp_ntoh16(conf->switch1.bootdelay);
	  conf->switch2.bootdelay = csp_ntoh16(conf->switch2.bootdelay);


	  conf->timing.wdt_timeout = csp_ntoh32(conf->timing.wdt_timeout);
	  conf->timing.com_tx_inhibit_timeout = csp_ntoh32(conf->timing.com_tx_inhibit_timeout);


	return status;
}


int hub_set_conf(nanohub_conf_t *globalconf) {
	nanohub_conf_t conf;
	memcpy(&conf, globalconf, sizeof(nanohub_conf_t));

	conf.knife0.time[0] = csp_hton16(conf.knife0.time[0]);
	conf.knife0.time[1] = csp_hton16(conf.knife0.time[1]);
	conf.knife0.bootdelay[0] = csp_hton32(conf.knife0.bootdelay[0]);
	conf.knife0.bootdelay[1] = csp_hton32(conf.knife0.bootdelay[1]);
	conf.knife0.delay[0] = csp_hton16(conf.knife0.delay[0]);
	conf.knife0.delay[1] = csp_hton16(conf.knife0.delay[1]);
	conf.knife0.enable[0] = conf.knife0.enable[0];
	conf.knife0.enable[1] = conf.knife0.enable[1];

	conf.knife1.time[0] = csp_hton16(conf.knife1.time[0]);
	conf.knife1.time[1] = csp_hton16(conf.knife1.time[1]);
	conf.knife1.bootdelay[0] = csp_hton32(conf.knife1.bootdelay[0]);
	conf.knife1.bootdelay[1] = csp_hton32(conf.knife1.bootdelay[1]);
	conf.knife1.delay[0] = csp_hton16(conf.knife1.delay[0]);
	conf.knife1.delay[1] = csp_hton16(conf.knife1.delay[1]);
	conf.knife1.enable[0] = conf.knife1.enable[0];
	conf.knife1.enable[1] = conf.knife1.enable[1];

	conf.switch0.bootdelay = csp_hton16(conf.switch0.bootdelay);
	conf.switch1.bootdelay = csp_hton16(conf.switch1.bootdelay);
	conf.switch2.bootdelay = csp_hton16(conf.switch2.bootdelay);

	conf.timing.wdt_timeout = csp_hton32(conf.timing.wdt_timeout);
	conf.timing.com_tx_inhibit_timeout = csp_hton32(conf.timing.com_tx_inhibit_timeout);

 	int status = csp_transaction(CSP_PRIO_NORM, node_hub, NANOHUB_PORT_CONF, 1000, &conf, sizeof(nanohub_conf_t), NULL, 0);


	return status;
}



int hub_get_adc_single(uint8_t channel, uint16_t *data) {
	int status;

	if (channel<8)
	{
		status = csp_transaction(CSP_PRIO_HIGH, node_hub, NANOHUB_PORT_ADC, 1000, &channel, 1, data, 2);
		printf("data %u\r\n",data[0]);
		data[0] = csp_ntoh16(data[0]);
		return status;
	}
	else
		return 0;
}


int hub_get_adc_all(uint16_t *data) {
	int status;
	uint8_t channel = 99;

	status = csp_transaction(CSP_PRIO_HIGH, node_hub, NANOHUB_PORT_ADC, 1000, &channel, 1, data, 16);
	data[0] = csp_ntoh16(data[0]);
	data[1] = csp_ntoh16(data[1]);
	data[2] = csp_ntoh16(data[2]);
	data[3] = csp_ntoh16(data[3]);
	data[4] = csp_ntoh16(data[4]);
	data[5] = csp_ntoh16(data[5]);
	data[6] = csp_ntoh16(data[6]);
	data[7] = csp_ntoh16(data[7]);
return status;
}


int hub_dio(uint8_t cmd, uint8_t iod, uint8_t pin, uint8_t val,uint8_t *ret) {
	int status;
	uint8_t dio[4];
	dio[0] = cmd;
	dio[1] = iod;
	dio[2] = pin;
	dio[3] = val;
	printf("%d %d %d %d\r\n",dio[0],dio[1],dio[2],dio[3]);

	status = csp_transaction(CSP_PRIO_HIGH, node_hub, NANOHUB_PORT_DIO, 1000, dio, 4, ret, 1);
	return status;
}


int hub_set_single_output(uint8_t channel, uint8_t mode, uint16_t delay, nanohub_switchdata_t *switchdata) {
	int status;
	nanohub_switch_t nanohub_switch;
	nanohub_switch.channel=channel;
	nanohub_switch.mode = mode;
	nanohub_switch.delay = csp_hton16(delay);

	status = csp_transaction(CSP_PRIO_HIGH, node_hub, NANOHUB_PORT_SET_SINGLE_SWITCH, 1000, &nanohub_switch, sizeof(nanohub_switch_t), switchdata, sizeof(nanohub_switchdata_t));
	if (status != sizeof(nanohub_switchdata_t))
		return status;

	switchdata->on_timer[0] = csp_ntoh16(switchdata->on_timer[0]);
	switchdata->on_timer[1] = csp_ntoh16(switchdata->on_timer[1]);
	switchdata->off_timer[0] = csp_ntoh16(switchdata->off_timer[0]);
	switchdata->off_timer[1] = csp_ntoh16(switchdata->off_timer[1]);
	switchdata->faults[0] = csp_ntoh16(switchdata->faults[0]);
	switchdata->faults[1] = csp_ntoh16(switchdata->faults[1]);
	return status;
}


int hub_knife(uint8_t knife, uint8_t channel, uint16_t delay, uint16_t duration,  nanohub_knivesdata_t *knivesdata) {
	int status;
	nanohub_knife_t nanohub_knife;
	nanohub_knife.knife = knife;
	nanohub_knife.channel = channel;
	nanohub_knife.duration = csp_hton16(duration);
	nanohub_knife.delay = csp_hton16(delay);

	status = csp_transaction(CSP_PRIO_HIGH, node_hub, NANOHUB_PORT_DEPLOY, 1000, &nanohub_knife, sizeof(nanohub_knife_t), knivesdata, sizeof(nanohub_knivesdata_t));
	if (status != sizeof(nanohub_knivesdata_t))
		return status;

	knivesdata->knife[0].burns[0] = csp_hton16(knivesdata->knife[0].burns[0]);
	knivesdata->knife[0].burns[1] = csp_hton16(knivesdata->knife[0].burns[1]);
	knivesdata->knife[0].timer[0] = csp_hton32(knivesdata->knife[0].timer[0]);
	knivesdata->knife[0].timer[1] = csp_hton32(knivesdata->knife[0].timer[1]);
	knivesdata->knife[1].burns[0] = csp_hton16(knivesdata->knife[1].burns[0]);
	knivesdata->knife[1].burns[1] = csp_hton16(knivesdata->knife[1].burns[1]);
	knivesdata->knife[1].timer[0] = csp_hton32(knivesdata->knife[1].timer[0]);
	knivesdata->knife[1].timer[1] = csp_hton32(knivesdata->knife[1].timer[1]);

	return status;
}



int hub_gyro(uint8_t cmd, uint8_t dest, uint8_t port, int32_t *gyrodata) {
	int status;
	uint8_t data[3];
	data[0] = cmd;
	data[1] = dest;
	data[2] = port;
	status = csp_transaction(CSP_PRIO_HIGH, node_hub, NANOHUB_PORT_GYRO, 1000, data, 3, gyrodata, sizeof(int32_t)*3);

	gyrodata[0] = csp_ntoh32(gyrodata[0]);
	gyrodata[1] = csp_ntoh32(gyrodata[1]);
	gyrodata[2] = csp_ntoh32(gyrodata[2]);
	return status;
}


int hub_reset_wdt(uint8_t magic, uint32_t *timer) {

	int status = csp_transaction(CSP_PRIO_NORM, node_hub, NANOHUB_PORT_RESET_WDT, 1000,&magic, 1, timer, sizeof(uint32_t));

	*timer = csp_ntoh32(*timer);

	return status;
}

void hub_hardload_conf(nanohub_conf_t *conf) {

	  conf->switch0.bootdelay = 0;
	  conf->switch1.bootdelay = 0;
	  conf->switch2.bootdelay = 100;

	  conf->dio.enable = 0;
	  conf->dio.dirmask = 0x00;
	  conf->dio.outputvalue = 0x00;
}

