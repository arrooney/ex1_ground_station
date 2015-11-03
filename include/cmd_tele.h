/*
 * cmd_tele.h
 *
 *  Created on: 07/07/2015
 *      Author: Alex Hamilton
 *      adhamilt@ualbeta.ca
 */

//#include "telecommand_packet.h"

#ifndef CMD_TELE_H_
#define CMD_TELE_H_

#define TELE_BLINK_SIZE			5
#define TELE_POWER_ON_SIZE			5
#define TELE_POST_EJECTION_SIZE			5
#define TELE_PRE_CHARGE_SIZE			5
#define TELE_START_UP_SIZE			5
#define TELE_POWER_SAFE_SIZE			5
#define TELE_DETUMBLE_SIZE			5
#define TELE_SCIENCE_SIZE			5
#define TELE_PS_UPPER_THRESH_SIZE			5
#define TELE_CONF_PS_UPPER_THRESH_SIZE			5
#define TELE_PS_LOWER_THRESH_SIZE			5
#define TELE_CONF_PS_LOWER_THRESH_SIZE			5
#define TELE_PC_UPPER_THRESH_SIZE			5
#define TELE_CONF_PC_UPPER_THRESH_SIZE			5
#define TELE_PC_LOWER_THRESH_SIZE			5
#define TELE_CONF_PC_LOWER_THRESH_SIZE			5
#define TELE_COMMAND_ADCS_SIZE			5
#define TELE_UPLOAD_SCRIPT_SIZE			5
#define TELE_DELETE_FILE_SIZE			5
#define TELE_CONF_DELETE_FILE_SIZE			5
#define TELE_QUERY_FILE_SIZE			5
#define TELE_DOWNLINK_FILE_SIZE			5
#define TELE_BOOT_IMAGE_SIZE			5
#define TELE_CONF_BOOT_IMAGE_SIZE			5
#define TELE_REBOOT_SIZE			5
#define TELE_CONF_REBOOT_SIZE			5
#define TELE_SYNC_RTC_SIZE			5
#define TELE_CONF_SYNC_RTC_SIZE			5
#define TELE_PRIORITIZE_DOWNLINK_SIZE			5
#define TELE_CONFIRM_TELEMETRY_SIZE			5
#define TELE_CONF_CONFIRM_TELEMETRY_SIZE			5
#define TELE_SCHEDULE_DOWNLINK_SIZE			5
#define TELE_CONF_SCHEDULE_DOWNLINK_SIZE			5
#define TELE_CONFIGURE_EPS_SIZE			5
#define TELE_CONF_CONFIGURE_EPS_SIZE			5
#define TELE_CONFIGURE_ADCS_SIZE			5
#define TELE_CONF_CONFIGURE_ADCS_SIZE			5
#define TELE_CONFIGURE_COMM_SIZE			5
#define TELE_CONF_CONFIGURE_COMM_SIZE			5

#define TELECOMMAND_PACKET_SIZE sizeof(telecommand_packet_t)

#endif /* CMD_TELE_H_ */
