#include "cmd_tele.h"
#include "nanomind.h"
#include "telecommand_packet.h"
#include "csp/csp.h"


#include <util/log.h>
#include <util/console.h>
#include <util/binaryprint.h>
#include <cmd.h>


int cmd_tele_blink(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_BLINK_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_BLINK;


	return CMD_ERROR_NONE;
}
int cmd_tele_power_on(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_POWER_ON_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_POWER_ON;


	return CMD_ERROR_NONE;
}
int cmd_tele_post_ejection(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_POST_EJECTION_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_POST_EJECTION;


	return CMD_ERROR_NONE;
}
int cmd_tele_pre_charge(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_PRE_CHARGE_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_PRE_CHARGE;


	return CMD_ERROR_NONE;
}
int cmd_tele_start_up(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_START_UP_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_START_UP;


	return CMD_ERROR_NONE;
}
int cmd_tele_power_safe(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_POWER_SAFE_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_POWER_SAFE;


	return CMD_ERROR_NONE;
}
int cmd_tele_detumble(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_DETUMBLE_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_DETUMBLE;


	return CMD_ERROR_NONE;
}
int cmd_tele_science(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_SCIENCE_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_SCIENCE;


	return CMD_ERROR_NONE;
}
int cmd_tele_ps_upper_thresh(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_PS_UPPER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_PS_UPPER_THRESH;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_ps_upper_thresh(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_PS_UPPER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_PS_UPPER_THRESH;


	return CMD_ERROR_NONE;
}
int cmd_tele_ps_lower_thresh(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_PS_LOWER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_PS_LOWER_THRESH;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_ps_lower_thresh(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_PS_LOWER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_PS_LOWER_THRESH;


	return CMD_ERROR_NONE;
}
int cmd_tele_pc_upper_thresh(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_PC_UPPER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_PC_UPPER_THRESH;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_pc_upper_thresh(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_PC_UPPER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_PC_UPPER_THRESH;


	return CMD_ERROR_NONE;
}
int cmd_tele_pc_lower_thresh(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_PC_LOWER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_PC_LOWER_THRESH;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_pc_lower_thresh(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_PC_LOWER_THRESH_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_PC_LOWER_THRESH;


	return CMD_ERROR_NONE;
}
int cmd_tele_command_adcs(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_COMMAND_ADCS_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_COMMAND_ADCS;


	return CMD_ERROR_NONE;
}
int cmd_tele_upload_script(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_UPLOAD_SCRIPT_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_UPLOAD_SCRIPT;


	return CMD_ERROR_NONE;
}
int cmd_tele_delete_file(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_DELETE_FILE_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_DELETE_FILE;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_delete_file(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_DELETE_FILE_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_DELETE_FILE;


	return CMD_ERROR_NONE;
}
int cmd_tele_query_file(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_QUERY_FILE_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_QUERY_FILE;


	return CMD_ERROR_NONE;
}
int cmd_tele_downlink_file(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_DOWNLINK_FILE_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_DOWNLINK_FILE;


	return CMD_ERROR_NONE;
}
int cmd_tele_boot_image(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_BOOT_IMAGE_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_BOOT_IMAGE;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_boot_image(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_BOOT_IMAGE_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_BOOT_IMAGE;


	return CMD_ERROR_NONE;
}
int cmd_tele_reboot(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_REBOOT_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_REBOOT;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_reboot(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_REBOOT_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_REBOOT;


	return CMD_ERROR_NONE;
}
int cmd_tele_sync_rtc(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_SYNC_RTC_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_SYNC_RTC;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_sync_rtc(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_SYNC_RTC_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_SYNC_RTC;


	return CMD_ERROR_NONE;
}
int cmd_tele_prioritize_downlink(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_PRIORITIZE_DOWNLINK_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_PRIORITIZE_DOWNLINK;


	return CMD_ERROR_NONE;
}
int cmd_tele_confirm_telemetry(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONFIRM_TELEMETRY_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONFIRM_TELEMETRY;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_confirm_telemetry(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_CONFIRM_TELEMETRY_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_CONFIRM_TELEMETRY;


	return CMD_ERROR_NONE;
}
int cmd_tele_schedule_downlink(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_SCHEDULE_DOWNLINK_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_SCHEDULE_DOWNLINK;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_schedule_downlink(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_SCHEDULE_DOWNLINK_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_SCHEDULE_DOWNLINK;


	return CMD_ERROR_NONE;
}
int cmd_tele_configure_eps(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONFIGURE_EPS_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONFIGURE_EPS;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_configure_eps(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_CONFIGURE_EPS_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_CONFIGURE_EPS;


	return CMD_ERROR_NONE;
}
int cmd_tele_configure_adcs(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONFIGURE_ADCS_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONFIGURE_ADCS;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_configure_adcs(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_CONFIGURE_ADCS_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_CONFIGURE_ADCS;


	return CMD_ERROR_NONE;
}
int cmd_tele_configure_comm(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONFIGURE_COMM_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONFIGURE_COMM;


	return CMD_ERROR_NONE;
}
int cmd_tele_conf_configure_comm(struct command_context *ctx) {

 	int status;
	csp_conn_t * conn=csp_connect(CSP_PRIO_NORM,1,OBC_PORT_TELECOMMAND,1000,CSP_TRANSACTION_SO);
	if(conn==NULL)
		return CMD_ERROR_FAIL;

	telecommand_packet_t tele;
	uint32_t cmdsize = TELE_CONF_CONFIGURE_COMM_SIZE + TELECOMMAND_PACKET_SIZE;

	tele.priority=0;
	tele.telecommand_code = TELECOMMAND_TYPE_CONF_CONFIGURE_COMM;


	return CMD_ERROR_NONE;
}
