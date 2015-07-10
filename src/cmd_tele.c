#include "cmd_tele.h"
#include "nanomind.h"
#include "telecommand_packet.h"
#include "csp/csp.h"

#include <stdlib.h>


#include <util/log.h>
#include <util/console.h>
#include <util/binaryprint.h>
#include <cmd.h>
#include "rtc.h"

//TODO: make this function produce the time appropriately
rtc_time_t get_rtc_date(char * date){
	rtc_time_t rtctime;
	return rtctime;

}


int cmd_tele_blink(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_BLINK_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_BLINK;
	tele.payload_bytes = TELE_BLINK_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_power_on(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_POWER_ON_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_POWER_ON;
	tele.payload_bytes = TELE_POWER_ON_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_post_ejection(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_POST_EJECTION_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_POST_EJECTION;
	tele.payload_bytes = TELE_POST_EJECTION_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_pre_charge(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_PRE_CHARGE_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_PRE_CHARGE;
	tele.payload_bytes = TELE_PRE_CHARGE_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_start_up(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_START_UP_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_START_UP;
	tele.payload_bytes = TELE_START_UP_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_power_safe(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_POWER_SAFE_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_POWER_SAFE;
	tele.payload_bytes = TELE_POWER_SAFE_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_detumble(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_DETUMBLE_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_DETUMBLE;
	tele.payload_bytes = TELE_DETUMBLE_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_science(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_SCIENCE_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_SCIENCE;
	tele.payload_bytes = TELE_SCIENCE_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_ps_upper_thresh(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_PS_UPPER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_PS_UPPER_THRESH;
	tele.payload_bytes = TELE_PS_UPPER_THRESH_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_ps_upper_thresh(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_PS_UPPER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_PS_UPPER_THRESH;
	tele.payload_bytes = TELE_CONF_PS_UPPER_THRESH_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_ps_lower_thresh(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_PS_LOWER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_PS_LOWER_THRESH;
	tele.payload_bytes = TELE_PS_LOWER_THRESH_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_ps_lower_thresh(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_PS_LOWER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_PS_LOWER_THRESH;
	tele.payload_bytes = TELE_CONF_PS_LOWER_THRESH_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_pc_upper_thresh(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_PC_UPPER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_PC_UPPER_THRESH;
	tele.payload_bytes = TELE_PC_UPPER_THRESH_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_pc_upper_thresh(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_PC_UPPER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_PC_UPPER_THRESH;
	tele.payload_bytes = TELE_CONF_PC_UPPER_THRESH_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_pc_lower_thresh(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_PC_LOWER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_PC_LOWER_THRESH;
	tele.payload_bytes = TELE_PC_LOWER_THRESH_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_pc_lower_thresh(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_PC_LOWER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_PC_LOWER_THRESH;
	tele.payload_bytes = TELE_CONF_PC_LOWER_THRESH_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_command_adcs(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_COMMAND_ADCS_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_COMMAND_ADCS;
	tele.payload_bytes = TELE_COMMAND_ADCS_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_upload_script(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_UPLOAD_SCRIPT_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_UPLOAD_SCRIPT;
	tele.payload_bytes = TELE_UPLOAD_SCRIPT_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_delete_file(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_DELETE_FILE_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_DELETE_FILE;
	tele.payload_bytes = TELE_DELETE_FILE_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_delete_file(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_DELETE_FILE_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_DELETE_FILE;
	tele.payload_bytes = TELE_CONF_DELETE_FILE_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_query_file(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_QUERY_FILE_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_QUERY_FILE;
	tele.payload_bytes = TELE_QUERY_FILE_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_downlink_file(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_DOWNLINK_FILE_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_DOWNLINK_FILE;
	tele.payload_bytes = TELE_DOWNLINK_FILE_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_boot_image(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_BOOT_IMAGE_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_BOOT_IMAGE;
	tele.payload_bytes = TELE_BOOT_IMAGE_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_boot_image(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_BOOT_IMAGE_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_BOOT_IMAGE;
	tele.payload_bytes = TELE_CONF_BOOT_IMAGE_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_reboot(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_REBOOT_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_REBOOT;
	tele.payload_bytes = TELE_REBOOT_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_reboot(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_REBOOT_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_REBOOT;
	tele.payload_bytes = TELE_CONF_REBOOT_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_sync_rtc(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_SYNC_RTC_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_SYNC_RTC;
	tele.payload_bytes = TELE_SYNC_RTC_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_sync_rtc(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_SYNC_RTC_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_SYNC_RTC;
	tele.payload_bytes = TELE_CONF_SYNC_RTC_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_prioritize_downlink(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_PRIORITIZE_DOWNLINK_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_PRIORITIZE_DOWNLINK;
	tele.payload_bytes = TELE_PRIORITIZE_DOWNLINK_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_confirm_telemetry(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONFIRM_TELEMETRY_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONFIRM_TELEMETRY;
	tele.payload_bytes = TELE_CONFIRM_TELEMETRY_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_confirm_telemetry(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_CONFIRM_TELEMETRY_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_CONFIRM_TELEMETRY;
	tele.payload_bytes = TELE_CONF_CONFIRM_TELEMETRY_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_schedule_downlink(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_SCHEDULE_DOWNLINK_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_SCHEDULE_DOWNLINK;
	tele.payload_bytes = TELE_SCHEDULE_DOWNLINK_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_schedule_downlink(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_SCHEDULE_DOWNLINK_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_SCHEDULE_DOWNLINK;
	tele.payload_bytes = TELE_CONF_SCHEDULE_DOWNLINK_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_configure_eps(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONFIGURE_EPS_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONFIGURE_EPS;
	tele.payload_bytes = TELE_CONFIGURE_EPS_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_configure_eps(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_CONFIGURE_EPS_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_CONFIGURE_EPS;
	tele.payload_bytes = TELE_CONF_CONFIGURE_EPS_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_configure_adcs(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONFIGURE_ADCS_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONFIGURE_ADCS;
	tele.payload_bytes = TELE_CONFIGURE_ADCS_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_configure_adcs(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_CONFIGURE_ADCS_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_CONFIGURE_ADCS;
	tele.payload_bytes = TELE_CONF_CONFIGURE_ADCS_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_configure_comm(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONFIGURE_COMM_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONFIGURE_COMM;
	tele.payload_bytes = TELE_CONFIGURE_COMM_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}
int cmd_tele_conf_configure_comm(struct command_context *ctx) {

	size_t siz;
	uint32_t i;
	uint32_t timeout = 0;
	const uint32_t cmdsize = TELE_CONF_CONFIGURE_COMM_SIZE + TELECOMMAND_PACKET_SIZE;
	telecommand_packet_t tele;
	uint8_t tflag,pflag,dflag,fflag,iflag =0;
	FILE * fid;
	uint8_t* buff[cmdsize];
	for(i=0;i<ctx->argc/2;i++){

		if(strncmp(ctx->argv[2*i],"-f",2)){
			fid=fopen(ctx->argv[(2*i)+1],"r");
			siz=fread(&buff[TELECOMMAND_PACKET_SIZE],1,(cmdsize-TELECOMMAND_PACKET_SIZE),fid);
			fclose(fid);
			if(siz<(cmdsize-TELECOMMAND_PACKET_SIZE)) return CMD_ERROR_SYNTAX;
			else fflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-i",2)){
			memcpy(&buff[TELECOMMAND_PACKET_SIZE],ctx->argv[(2*i)+1],(cmdsize-TELECOMMAND_PACKET_SIZE));
			iflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-t",2)){
			timeout=atoi(ctx->argv[(2*i)+1]);
			tflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-p",2)){
			tele.priority=atoi(ctx->argv[(2*i)+1]);
			pflag=1;
		}
		else if(strncmp(ctx->argv[2*i],"-d",2)){
			tele.exec_date=get_rtc_date(ctx->argv[(2*i)+1]);
			dflag=1;
		}
		else{
			return CMD_ERROR_SYNTAX;
		}

	}


	if((fflag|iflag)==0){
		return CMD_ERROR_SYNTAX;
	}
	if(tflag==0) timeout=1000;
	if(pflag==0) tele.priority=0;
	if(dflag==0) tele.exec_date=get_rtc_date("0");





	tele.telecommand_code = TELECOMMAND_TYPE_CONF_CONFIGURE_COMM;
	tele.payload_bytes = TELE_CONF_CONFIGURE_COMM_SIZE;



	memcpy(buff,&tele,TELECOMMAND_PACKET_SIZE);

	csp_transaction(CSP_PRIO_NORM, NODE_OBC,OBC_PORT_TELECOMMAND,timeout,buff,cmdsize,NULL,0);

	return CMD_ERROR_NONE;
}

command_t __sub_command tele_subcommands[] = {
	{
		.name="blink",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_blink,
	},{
		.name="power_on",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_power_on,
	},{
		.name="post_ejection",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_post_ejection,
	},{
		.name="pre_charge",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_pre_charge,
	},{
		.name="start_up",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_start_up,
	},{
		.name="power_safe",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_power_safe,
	},{
		.name="detumble",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_detumble,
	},{
		.name="science",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_science,
	},{
		.name="ps_upper_thresh",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_ps_upper_thresh,
	},{
		.name="conf_ps_upper_thresh",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_ps_upper_thresh,
	},{
		.name="ps_lower_thresh",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_ps_lower_thresh,
	},{
		.name="conf_ps_lower_thresh",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_ps_lower_thresh,
	},{
		.name="pc_upper_thresh",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_pc_upper_thresh,
	},{
		.name="conf_pc_upper_thresh",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_pc_upper_thresh,
	},{
		.name="pc_lower_thresh",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_pc_lower_thresh,
	},{
		.name="conf_pc_lower_thresh",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_pc_lower_thresh,
	},{
		.name="command_adcs",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_command_adcs,
	},{
		.name="upload_script",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_upload_script,
	},{
		.name="delete_file",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_delete_file,
	},{
		.name="conf_delete_file",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_delete_file,
	},{
		.name="query_file",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_query_file,
	},{
		.name="downlink_file",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_downlink_file,
	},{
		.name="boot_image",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_boot_image,
	},{
		.name="conf_boot_image",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_boot_image,
	},{
		.name="reboot",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_reboot,
	},{
		.name="conf_reboot",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_reboot,
	},{
		.name="sync_rtc",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_sync_rtc,
	},{
		.name="conf_sync_rtc",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_sync_rtc,
	},{
		.name="prioritize_downlink",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_prioritize_downlink,
	},{
		.name="confirm_telemetry",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_confirm_telemetry,
	},{
		.name="conf_confirm_telemetry",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_confirm_telemetry,
	},{
		.name="schedule_downlink",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_schedule_downlink,
	},{
		.name="conf_schedule_downlink",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_schedule_downlink,
	},{
		.name="configure_eps",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_configure_eps,
	},{
		.name="conf_configure_eps",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_configure_eps,
	},{
		.name="configure_adcs",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_configure_adcs,
	},{
		.name="conf_configure_adcs",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_configure_adcs,
	},{
		.name="configure_comm",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_configure_comm,
	},{
		.name="conf_configure_comm",
		.help = " ",
		.usage = "<-f file | -i input...> [-d date -t timeout -p priority]",
		.handler = cmd_tele_conf_configure_comm,
	}
};

command_t __root_command tele_commands[] = {
	{
		.name = "tele",
		.help = "Telecommands",
		.chain = INIT_CHAIN(tele_subcommands),
	}
};

void cmd_tele_setup(void) {
	command_register(tele_commands);
}
