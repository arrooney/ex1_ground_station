
include gom2.fth
include dispatch.fth

\ ******************************************************************************\
\ Helper words									\
\ ******************************************************************************\
: CHECK-ERR ( n -- n , Checks error code on the stack without removing it )
	DUP
	GOM.ERR.OK = NOT
	IF
		CR ." gom error: " DUP . CR
	THEN
;

: WDT ( -- , resets the EPS and COM watchdogs )
	S" eps resetwdt" GOM
	S" ax100_rst_wdt" GOM
;

: DOWN ( -- , downloads the file of the night from SD card only )
	S" ftp download_file /boot/state_log.bin" GOM
;

: FIX ( -- , try to fix shit. )
	S" ftp rm /boot/debug_radio.txt" GOM
	WAIT
	S" reboot 1" GOM
	WAIT
	S" ftp upload_file safemode.bin /boot/safemode.bin" GOM
	WAIT
;


: SOMETHING ( -- , downloads the file of the night from SD card only )
	S" ftp upload_file lsbo_tl.txt /sd/lsbo_tl.txt" GOM
;

: NEWRAM ( -- , downloads the file of the night from SD card only )
	S" ftp upload_file nanomind_Oct16.lzo /boot/nanomind.bin" GOM
;

: RAM ( -- , uploads boot.conf to allow use of the RAM image )
	S" ftp upload_file boot.conf /boot/boot.conf" GOM
;

: NORAM ( -- , removes boot.conf in case of shit )
	S" ftp rm /boot/boot.conf" GOM
;

: SAFE.UP ( -- , uploads the safemode.bin file to enable safemode )
	S" ftp upload_file safemode.bin /boot/safemode.bin" GOM
;

: SAFE.DOWN ( -- , removes the safemode.bin file and allows normal boot. )
	S" ftp rm /boot/safemode.bin" GOM
;

: DEBUG.UP ( -- , removes the safemode.bin file and allows normal boot. )
	S" ftp upload_file debug_radio.txt /boot/debug_radio.txt" GOM
;

: DEBUG.DOWN ( -- , removes the debug_radio.txt file to silence debugs. )
	S" ftp rm /boot/debug_radio.txt" GOM
;

: FILEFORMAT ( -- , downloads the file of the night from SD card only )
	S" ftp backend 1" GOM
	S" ftp mkfs 0" GOM
;

: DOWN&RM ( -- , downloads the file and then deletes after with user input)
	S" ftp download_file /sd/006W0126.bin" GOM
	WAIT
	KEY
	[CHAR] d = IF
		." Removing" CR
		S" ftp rm /sd/006W0126.bin" GOM
	THEN
;

: RM ( -- , remove the file of the night. )
	S" ftp rm /boot/bootcause.bin" GOM
;

: MNLP.UP.1 ( -- , Upload mnlp script to slot 0. Must be empty first. )
	S" ftp upload_file CA03_2017WK42_D1_SCRIPT.bin /sd/MNLP_0.bin" GOM
;

: MNLP.UP.2 ( -- , Upload mnlp script to slot 1. Must be empty first. )
	S" ftp upload_file CA03_2017WK42_D2_SCRIPT.bin /sd/MNLP_1.bin" GOM
;

: MNLP.UP.3 ( -- , Upload mnlp script to slot 2. Must be empty first. )
	S" ftp upload_file CA03_2017WK42_D3_SCRIPT.bin /sd/MNLP_2.bin" GOM
;

: MNLP.UP.4 ( -- , Upload mnlp script to slot 3. Must be empty first. )
	S" ftp upload_file CA03_2017WK42_D4_SCRIPT.bin /sd/MNLP_3.bin" GOM
;

: MNLP.UP.5 ( -- , Upload mnlp script to slot 4. Must be empty first. )
	S" ftp upload_file CA03_2017WK42_D5_SCRIPT.bin /sd/MNLP_4.bin" GOM
;

: MNLP.UP.6 ( -- , Upload mnlp script to slot 5. Must be empty first. )
	S" ftp upload_file CA03_2017WK42_D6_SCRIPT.bin /sd/MNLP_5.bin" GOM
;

: MNLP.UP.7 ( -- , Upload mnlp script to slot 6. Must be empty first. )
	S" ftp upload_file CA03_2017WK42_D7_SCRIPT.bin /sd/MNLP_6.bin" GOM
;

: MNLP.DOWN ( -- , downloads the named MNLP file )
	S" ftp download_file /boot/M170922S.bin" GOM
;

: MNLP.RM ( -- , removes the specified MNLP script )
	S" ftp rm /sd/MNLP_5.bin" GOM
;

: MNLP.OFF ( -- , shuts off MNLP task. )
	S" mnlp stop" GOM.COMMAND
	CHECK-ERR
;

: MNLP.ON ( -- , restarts MNLP task. )
	S" mnlp start" GOM.COMMAND
	CHECK-ERR
;


: PING ( -- , send a ping to the nanomind )
	S" ping 1 1200 10" GOM
;


: PING.COM ( -- , send a ping to the nanocom )
	S" ping 5 1200 10" GOM
;


: PING.EPS ( -- , send a ping to the nanopower )
	S" ping 2 1200 10" GOM
;

: PING.HUB ( -- , send a ping to the nanohub )
        S" ping 3 1200 10" GOM
;

: RTOS.STAT
    S" rtos info" GOM.COMMAND
; 

\ ******************************************************************************\
\ ADCS general words								\
\ ******************************************************************************\
30 $VAR adcs_flush_n
30 $VAR adcs_flush_cmd
: ADCS.FLUSH ( n -- , n = number of bytes to flush from ADCS USART )
    adcs_flush_n $CLEAR
    adcs_flush_cmd $CLEAR

    adcs_flush_n 2 PICK >STRING
    S" adcs flush|" adcs_flush_cmd $!
    adcs_flush_cmd adcs_flush_n $APPEND

    adcs_flush_cmd GOM.COMMAND
;

: ADCS.RESET ( -- , resets the adcs. Wait 15 seconds for it to boot. )
	S" obc adcs 1 1 reset.txt" GOM
;

: ADCS.RM ( -- , deletes the adcs.bin file from /boot/ )
	S" ftp rm /boot/adcs.bin" GOM
;

: ADCS.HARDRESET ( -- , turns off and on power to ADCS. Wait 15 seconds for boot. )
	S" eps output 0 0 0" GOM
	S" eps output 3 0 0" GOM
	WAIT
	S" eps hk" GOM
	WAIT
	S" eps output 3 1 0" GOM
	S" eps output 0 1 0" GOM
	WAIT
	S" eps hk" GOM
	WAIT
	s" obc adcs 128 8 null.txt" GOM
	WAIT
;

: ADCS.GET.RTC ( -- , gets the clock from the ADCS OBC RTC )
	S" obc adcs 143 6 null.txt" GOM
;

: ADCS.CRC ( -- , polls telemetry 241 to reset the file download to nanomind )
	S" obc adcs 241 6 null.txt" GOM
;

: ADCS.FORMAT.FS ( -- , delete all files in the ADCS sd card! )
	S" obc adcs 119 0 null.txt" GOM
	WAIT
	s" obc adcs 129 6 null.txt" GOM
	WAIT
	s" obc adcs 130 6 null.txt" GOM
	WAIT
	s" obc adcs 131 4 null.txt" GOM
	WAIT
;

: ADCS.TAKE.PHOTO ( -- , take photo. Currently nadir camera. )
	S" obc adcs 110 10 take_photo_nadir.txt" GOM
	WAIT
	s" obc adcs 230 16 null.txt" GOM
	WAIT
;

: ADCS.LOG.START ( -- , Starts the telemetry log. )
	s" obc adcs 128 8 null.txt" GOM
	WAIT
	s" obc adcs 5 5 power_minimum.txt" GOM
	WAIT
	s" obc adcs 3 1 mode_run.txt" GOM
	WAIT
	s" obc adcs 17 1 estimate_rate_pitch.txt" GOM
	WAIT
	s" obc adcs 4 13 log_telem_3axis.txt" GOM
	WAIT
;

: ADCS.LOG.SHORT ( -- , Starts the telemetry log, but only the log command. )
	s" obc adcs 128 8 null.txt" GOM
	WAIT
	s" obc adcs 4 13 log_telem_3axis.txt" GOM
	WAIT
;

: ADCS.LOG.STOP ( -- , Stops the telemetry log )
	s" obc adcs 4 13 stop_telem_step1.txt" GOM
	WAIT
	s" obc adcs 114 0 null.txt" GOM
	WAIT
	s" obc adcs 240 22 null.txt" GOM
	WAIT
;

: ADCS.FIND ( -- , Finds the files in the SD card )
	s" obc adcs 128 8 null.txt" GOM
	WAIT
	s" obc adcs 114 0 null.txt" GOM
	WAIT
	s" obc adcs 240 22 null.txt" GOM
	WAIT
;

: ADCS.DOWNLOAD.0 ( -- , Downloads 1st telem file to Nanomind )
	s" obc adcs 128 8 null.txt" GOM
	WAIT
	s" obc adcs 114 0 null.txt" GOM
	WAIT
	s" obc adcs 240 22 null.txt" GOM
	WAIT
	s" obc adcs 116 13 download_telem_00.txt" GOM
	WAIT
	s" obc adcs 241 6 null.txt" GOM
	WAIT
	s" obc adcs 242 0 null.txt" GOM
	WAIT
;

\ ******************************************************************************\
\ ADCS sensor telemetry words								\
\ ******************************************************************************\

: ADCS.CSS ( -- , gets the raw coarse sun sensor values from the ADCS )
	S" obc adcs 166 6 null.txt" GOM
	WAIT
	S" obc adcs 165 6 null.txt" GOM
	WAIT
;

: ADCS.SUN.CAL ( -- , gets the predicted sun, calibrated css and fine sun vectors )
	S" obc adcs 158 6 null.txt" GOM
	WAIT
	S" obc adcs 150 6 null.txt" GOM
	WAIT
	S" obc adcs 151 6 null.txt" GOM
	WAIT
;

: ADCS.IGRF ( -- , gets the IGRF model vector )
	S" obc adcs 157 6 null.txt" GOM
	WAIT
;

: ADCS.MAG ( -- , gets the calibrated magnetometer xyz, and then the SGP predicted values. )
	S" obc adcs 149 6 null.txt" GOM
	WAIT
	S" obc adcs 157 6 null.txt" GOM
;

: ADCS.MEMS ( -- , gets the mems rate sensor measurement. )
	S" obc adcs 153 6 null.txt" GOM
;

: ADCS.WHEEL ( -- , gets the wheel spead measurement. )
	S" obc adcs 154 6 null.txt" GOM
;

: ADCS.GPS.STATUS ( -- , gets GPS status from ADCS )
	S" obc adcs 168 6 null.txt" GOM
;

: ADCS.GPS.TIME ( -- , gets GPS time from ADCS )
	S" obc adcs 169 6 null.txt" GOM
;

: ADCS.GPS.XYZ ( -- , gets GPS spatials from ADCS )
	S" obc adcs 170 6 null.txt" GOM
	WAIT
	S" obc adcs 171 6 null.txt" GOM
	WAIT
	S" obc adcs 172 6 null.txt" GOM
;

: ADCS.MAG.TEMP ( -- , gets the a few housekeeping values, including mag temp. )
	S" obc adcs 175 6 null.txt" GOM
	WAIT
;

: ADCS.TLE ( -- , gets the TLE parameters saved in the config. )
	S" obc adcs 191 64 null.txt" GOM
	WAIT
;

: ADCS.CONF ( -- , gets the ADCS Config. )
	S" obc adcs 192 240 null.txt" GOM
	WAIT
;

: ADCS.B1 ( -- , gets ADCS boot and running program status. )
	S" obc adcs 129 6 null.txt" GOM
	WAIT
;

: ADCS.B2 ( -- , Coomunication status for CubeACP. )
	S" obc adcs 130 6 null.txt" GOM
	WAIT
;

: ADCS.B3 ( -- , Telemetry frame with acknowledge of previous command send. )
	S" obc adcs 131 4 null.txt" GOM
	WAIT
;

: ADCS.STATUS ( -- , gets the current state of the ADCS solution. )
	S" obc adcs 136 48 null.txt" GOM
	WAIT
;



\ ******************************************************************************\
\ ADCS actuator control words							\
\ ******************************************************************************\

: ADCS.WHEEL.ON ( -- , turns the wheel on to the selected speed. )
	S" obc adcs 32 6 wheel_speed_4140.txt" GOM
;

: ADCS.WHEEL.OFF ( -- , turns wheel off with speed set to 0. )
	S" obc adcs 32 6 wheel_speed_0.txt" GOM
;

: ADCS.DEPLOY ( -- , attempts mag deployment with timeout in txt file. )
	S" obc adcs 6 1 deploy_mag_20.txt" GOM
;

: ADCS.DETUMBLE ( -- , puts ADCS into the detumble control mode. )
	S" obc adcs 18 4 detumble_forever.txt" GOM
	WAIT
;

: ADCS.DETUMBLE.HIGHRATE ( -- , puts ADCS into the high rate detumble control mode >30 deg/s. )
	S" obc adcs 18 4 detumble_highrate_forever.txt" GOM
	WAIT
;

: ADCS.YMOM ( -- , puts ADCS into y momentum control mode. )
	S" obc adcs 18 4 ymomentum_forever.txt" GOM
	WAIT
;

: ADCS.YMOM.FORCED ( -- , puts ADCS into y momentum control mode, ignoring the bounds.. )
	S" obc adcs 18 4 ymom_forced_forever.txt" GOM
	WAIT
;

: ADCS.NOCONTROL ( -- , stops all ADCS control. )
	S" obc adcs 18 4 no_control.txt" GOM
	WAIT
;

: ADCS.EKF ( -- , enables EKF estimation. )
	S" obc adcs 17 1 estimate_ekf.txt" GOM
	WAIT
;

: ADCS.MAGRATE ( -- , enables magrate estimation. )
	S" obc adcs 17 1 estimate_magrate.txt" GOM
	WAIT
;

: ADCS.RATEPITCH ( -- , enables rate pitch estimation. )
	S" obc adcs 17 1 estimate_rate_pitch.txt" GOM
	WAIT
;

: ADCS.TEST ( -- , multistep command to test no rate detumble with EKF. )
	S" obc adcs 128 8 null.txt" GOM
	WAIT	
	S" obc adcs 64 64 tle_adcs_oct18.txt" GOM
	WAIT
	S" obc adcs 91 26 est_param_config_v2.txt" GOM
	WAIT
	S" obc adcs 90 24 inertia_config.txt" GOM
	WAIT
	S" obc adcs 17 1 estimate_rate_pitch.txt" GOM
	WAIT
	S" obc adcs 136 48 null.txt" GOM
	WAIT
	S" obc adcs 18 4 detumble_forever.txt" GOM
	WAIT
	S" obc adcs 136 48 null.txt" GOM
	WAIT
;


\ ******************************************************************************\
\ ADCS config words								\
\ ******************************************************************************\
: ADCS.SET.CONFIG ( -- , updates configs that change or get deleted)
	s" obc adcs 128 8 null.txt" GOM
	WAIT
	S" obc adcs 91 26 est_param_config_v2.txt" GOM
	WAIT
	S" obc adcs 90 24 inertia_config.txt" GOM
	WAIT
	S" obc adcs 86 30 mag_config_v4.txt" GOM
	WAIT
	S" obc adcs 83 14 css_config_v2.txt" GOM
	WAIT
	S" obc adcs 84 17 sun_config_v1.txt" GOM
	WAIT
	S" obc adcs 87 6 rot_sens_config.txt" GOM
	WAIT
	S" obc adcs 82 13 wheel_config.txt" GOM
	WAIT
	S" obc adcs 88 10 detumble_cont_config.txt" GOM
	WAIT
	S" obc adcs 81 13 torq_config_v2.1.txt" GOM
	WAIT
	S" obc adcs 64 64 tle_adcs_oct18.txt" GOM
	WAIT
	S" obc adcs 2 6 null.txt" GOM
	WAIT
	S" obc adcs 136 48 null.txt" GOM
	WAIT
;


: ADCS.SET.RTC ( -- , updates the clock with computer time)
	S" obc adcs 2 6 null.txt" GOM
;

: ADCS.SAVE.CONFIG ( -- , writes config from ram to boot file )
	S" obc adcs 100 0 null.txt" GOM
	WAIT
;

: ADCS.SET.ESTPARAM ( -- , writes config for estimation parameters )
	S" obc adcs 91 26 est_param_config_v3_sun_css.txt" GOM
	WAIT
;

: ADCS.SET.NADIR ( -- , uploads nadir camera settings )
	S" obc adcs 85 57 nadir_eauto.txt" GOM
;

: ADCS.SET.SUN ( -- , uploads sun camera settings )
	S" obc adcs 84 17 sun_config_v1.txt" GOM
;

: ADCS.SET.TORQ ( -- , uploads magnetotorquer settings )
	S" obc adcs 81 13 torq_config_v2.1.txt" GOM
;

: ADCS.SET.WHEEL ( -- , uploads wheel settings )
	S" obc adcs 82 13 wheel_config.txt" GOM
;

: ADCS.SET.MAG ( -- , uploads mag settings )
	S" obc adcs 86 30 mag_config_v4.txt" GOM
;

: ADCS.SET.CSS ( -- , uploads CSS settings )
	S" obc adcs 83 14 css_config.txt" GOM
;

: ADCS.SET.RATESENSOR ( -- , uploads rate sensor settings )
	S" obc adcs 87 6 rot_sens_config.txt" GOM
;

: ADCS.SET.INERTIA ( -- , uploads moment of inertia table)
	S" obc adcs 90 24 inertia_config.txt" GOM
;

: ADCS.SET.DETUMBLE_CONTROL ( -- , uploads detumble control parameters)
	S" obc adcs 88 10 detumble_cont_config.txt" GOM
;

: ADCS.SET.TLE ( -- , uploads TLE to adcs )
	S" obc adcs 64 64 tle_adcs_oct18.txt" GOM
;

\ ******************************************************************************\
\ Satellite subsystem words								\
\ ******************************************************************************\

: EPS.GPS.ON ( -- , turns on the GPS power output )
	S" eps output 4 1 0" GOM
;

: EPS.GPS.OFF ( -- , turns off the GPS power output )
	S" eps output 4 0 0" GOM
;

: UDOS.START ( -- , Start the dosimeter logger )
	S" udos|start" GOM.COMMAND
	CHECK-ERR
;

: UDOS.STOP ( -- , Stop the dosimeter logger )
	S" udos|stop" GOM.COMMAND
	CHECK-ERR
;

: HUB.KNIFE ( -- , Attempts deployment of knives. knife, channel, delay, duration )
	S" hub knife 0 2 300 400" GOM
;

: HUB.KNIFE.GET ( -- , Gets knife info from hub )
	S" hub knife 99" GOM
;

: RADIO.SILENCE ( -- , Sets the tx_inhibit rparam to the value of seconds. )
	S" rparam init 5 0" GOM
	S" rparam set tx_inhibit 60" GOM
;

: RADIO.RAMUP.GO
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	WAIT
	s" rparam init 5 0" GOM
	s" rparam set rx_baud 12000" GOM
	WAIT
	s" rparam init 15 0" GOM
	s" rparam set tx_baud 12000" GOM
	WAIT
;

: RADIO.RAMUP.STOP
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	WAIT
	s" rparam init 5 0" GOM
	s" rparam set rx_baud 4800" GOM
	WAIT
	s" rparam init 15 0" GOM
	s" rparam set tx_baud 4800" GOM
	WAIT
;

: RADIO.RAMDOWN.GO
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	WAIT
	s" rparam init 15 0" GOM
	s" rparam set rx_baud 12000" GOM
	WAIT
	s" rparam init 5 0" GOM
	s" rparam set tx_baud 12000" GOM
	WAIT
;

: RADIO.RAMDOWN.STOP
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	WAIT
	s" rparam init 15 0" GOM
	s" rparam set rx_baud 4800" GOM
	WAIT
	s" rparam init 5 0" GOM
	s" rparam set tx_baud 4800" GOM
	WAIT
;

: RADIO.5
	s" rparam init 15 0" GOM
	s" rparam set tx_baud 4800" GOM
	WAIT
	s" rparam init 5 0" GOM
	s" rparam set rx_baud 9600" GOM
	WAIT
	s" rparam init 15 0" GOM
	s" rparam set tx_baud 9600" GOM
	WAIT
;

: RADIO.4
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	WAIT
	s" rparam init 15 0" GOM
	s" rparam set rx_baud 28800" GOM
	WAIT
	s" rparam init 5 0" GOM
	s" rparam set tx_baud 28800" GOM
	WAIT
;

: RADIO.3
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	WAIT
	s" rparam init 15 0" GOM
	s" rparam set rx_baud 19200" GOM
	WAIT
	s" rparam init 5 0" GOM
	s" rparam set tx_baud 19200" GOM
	WAIT
;

: RADIO.2
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	WAIT
	s" rparam init 15 0" GOM
	s" rparam set rx_baud 9600" GOM
	WAIT
	s" rparam init 5 0" GOM
	s" rparam set tx_baud 9600" GOM
	WAIT
;

: RADIO.1
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	WAIT
	s" rparam init 15 0" GOM
	s" rparam set rx_baud 4800" GOM
	WAIT
	s" rparam init 5 0" GOM
	s" rparam set tx_baud 4800" GOM
	WAIT
;
: RADIO.1a
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	s" rparam init 15 0" GOM
	s" rparam set rx_baud 4800" GOM
	s" rparam init 5 0" GOM
	s" rparam set tx_baud 4800" GOM
;

: RADIO.2a
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	s" rparam init 15 0" GOM
	s" rparam set rx_baud 9600" GOM
	s" rparam init 5 0" GOM
	s" rparam set tx_baud 9600" GOM
;

: RADIO.3a
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	s" rparam init 15 0" GOM
	s" rparam set rx_baud 19200" GOM
	s" rparam init 5 0" GOM
	s" rparam set tx_baud 19200" GOM
;

: RADIO.4a
	s" rparam init 5 0" GOM
	s" rparam set reboot_in 600" GOM
	s" rparam init 15 0" GOM
	s" rparam set rx_baud 28800" GOM
	s" rparam init 5 0" GOM
	s" rparam set tx_baud 28800" GOM
;

: RADIO.5a
	s" rparam init 15 0" GOM
	s" rparam set tx_baud 4800" GOM
	s" rparam init 5 0" GOM
	s" rparam set rx_baud 9600" GOM
	s" rparam init 15 0" GOM
	s" rparam set tx_baud 9600" GOM
;


\ ******************************************************************************\
\ FTP words									\
\ ******************************************************************************\
: LS-SD ( -- , list real time contents of SD card. WARNING this takes a long time to print )	
	S" ftp ls /sd/" GOM
;

: LS-BOOT ( -- , list real time contents of flash )	
	S" ftp ls /boot/" GOM
;


\ ******************************************************************************\
\ Ring buffer words								\
\ ******************************************************************************\
30 $VAR ring-entry
30 $VAR file-entry
: RING.CLEAN
    ( addr n1 n2 -- , delete n2 ring buffer entries starting at addr,n1.
    Assumes backend 2
    ex: S" 004R0004.bin" 3 RING.CLEAN
    This will remove 004, 005, and 006 from the R ring buffer. )
    2 PICK 2 PICK ring-entry $!

    0 DO \ Loop n2 times
	S" /sd/" file-entry $!
	file-entry ring-entry $APPEND
	file-entry GOM.FTP.REMOVE
	ring-entry NEXT-RING DROP DROP
    LOOP
    DROP DROP
;

: RING.DOWNLOAD ( n1, n2 -- , Download n1 files from ring buffer n2 )
	SWAP 0 DO
		DUP GOM.RING.DOWNLOAD
		GOM.ERR.OK = NOT IF
			." Error downloading at index: " I . CR
			." Stack trace: " .S CR
		THEN
	LOOP
	." Done"
;

: RING.MOVE ( n1, n2, n3, n4 -- , n1 = number of times to increment or decrement
    				  n2 = ring buffer to change
    				  n3 = -1 will change tail, 1 will change head
				  n4 = -1 will decrement, 1 will increment )
    3 PICK
    0 DO
	3DUP GOM.RING.MOVE DROP
    LOOP
    DROP DROP DROP DROP
;

: RING.FETCH ( -- , Fetch the tail of every ring buffer )
	GOM.RING.FETCH-ALL-TAILS DUP
	GOM.ERR.OK = NOT IF
		." Error fetching tails"
	ELSE
		DROP
		." Fetch successful"
	THEN
;

: RING.UPDATE ( -- , Force satellite to update ring buffer meta data )
    RING.FETCH
;


20 $VAR ring.hold
40 $VAR ring.buf
: RING.RESIZE ( addr n1 n2 n3 -- ,
    addr n1 = string, the name of the ring buffer to resize, ex, "wod", "dfgm raw"
    n2 = new capacity
    n3 = new file size
    ex: S" wod" 3 266 RING.RESIZE -- wod ring buffer will have a max 3 files
    with each file having a max size of 266 bytes
    )

    3 pick 3 pick ring.hold $!
    S" RB|" ring.buf $!
    ring.buf ring.hold $APPEND
    S" :" ring.hold $!
    ring.buf ring.hold $APPEND

    ring.hold 3 PICK >STRING
    ring.buf ring.hold $APPEND

    S" :" ring.hold $!
    ring.buf ring.hold $APPEND

    ring.hold 2 PICK >STRING
    ring.buf ring.hold $APPEND
    DROP DROP DROP DROP
    
    ring.buf GOM.COMMAND
;

: RING.FLUSH ( -- , run ring buffer flush command )
    S" RB|flush" GOM.COMMAND
;

: RING.COMMIT ( -- , rung ring buffer commit command )
    S" RB|commit" GOM.COMMAND
;

: RING.HELP ( -- , run ring buffer help command )
    S" RB" GOM.COMMAND
;

: RING.CURRENT ( -- , run ring buffer current size command )
    S" RB|current" GOM.COMMAND
;

: RING.BUGFIX-TEMP
    CR S" This will turn off the DFGM, continue? [WAIT]" TYPE WAIT
    S" wod" 60 31440 RING.RESIZE
    S" dfgm raw" 150 4096000 RING.RESIZE
    S" dfgm hk" 30 6912 RING.RESIZE
    S" dfgm filt1" 100 86880 RING.RESIZE
    S" dfgm filt2" 100 218880 RING.RESIZE
    S" athena" 48 102400 RING.RESIZE
    RING.FLUSH
    CR S" Abort if there were errors [WAIT]" TYPE WAIT
    CR S" Proceed immediately after the next beacon [WAIT]" TYPE WAIT
    S" dfgm power|off" GOM.COMMAND
    S" ftp rm /sd/MT-WLog.txt" GOM
    S" ftp rm /sd/MT-RLog.txt" GOM
    S" ftp rm /sd/MT-ALog.txt" GOM
    S" ftp rm /sd/MT-BLog.txt" GOM
    S" ftp rm /sd/MT-HLog.txt" GOM
    S" ftp rm /sd/MT_KLog.txt" GOM
    S" reboot 1" GOM
;


\ ******************************************************************************\
\ WOD words									\
\ ******************************************************************************\
: WOD.FETCH ( -- , Fetch head and tail of wod ring buffer )
    GOM.RING.WOD GOM.RING.FETCH
;    

: WOD.DOWNLOAD ( n -- , Download n WOD files )
        GOM.RING.WOD RING.DOWNLOAD
;

: WOD.TAIL-INC ( n -- , Increment tail n times )
    GOM.RING.WOD -1 1 RING.MOVE
;

: WOD.TAIL-DEC ( n -- , Decrement tail n times )
    GOM.RING.WOD -1 -1 RING.MOVE
;

: WOD.HEAD-INC ( n -- , Increment head n times )
    GOM.RING.WOD 1 1 RING.MOVE
;

: WOD.HEAD-INC ( n -- , Decrement head n times )
    GOM.RING.WOD 1 -1 RING.MOVE
;

\ ******************************************************************************\
\ EPS words									\
\ ******************************************************************************\
: EPS.HK ( -- , Print real time EPS housekeeping )
  	S" eps hk get" GOM
;	 


\ ******************************************************************************\
\ DFGM words									\
\ ******************************************************************************\
: DFGM.DIAG ( -- n , Print real time diagnostics about dfgm. Puts error code on stack )
	S" dfgm diag" GOM.COMMAND DUP
	CHECK-ERR
;

: DFGM.ON ( -- n , Turn on dfgm in real time. Puts error code on stack )
	S" dfgm power|on" GOM.COMMAND
	CHECK-ERR
;

: DFGM.OFF ( -- n , Turn off dfgm in real time. Puts error code on stack )
	S" dfgm power|off" GOM.COMMAND
	CHECK-ERR
;

: DFGM.RAW.ON
    S" dfgm stream|raw on" GOM.COMMAND
    CHECK-ERR
;

: DFGM.RAW.OFF
    S" dfgm stream|raw off" GOM.COMMAND
    CHECK-ERR
;

: DFGM.FILT1.ON
    S" dfgm stream|filt1 on" GOM.COMMAND
    CHECK-ERR
;

: DFGM.FILT1.OFF
    S" dfgm stream|filt1 off" GOM.COMMAND
    CHECK-ERR
;

: DFGM.FILT2.ON
    S" dfgm stream|filt2 on" GOM.COMMAND
    CHECK-ERR
;

: DFGM.FILT2.OFF
    S" dfgm stream|filt2 off" GOM.COMMAND
    CHECK-ERR
;

: DFGM.RAW.DOWNLOAD ( n -- , Download n raw DFGM files )
	GOM.RING.DFGM-RAW RING.DOWNLOAD
;

: DFGM.RAW.FETCH ( -- , Fetch head and tail of DFGM raw ring buffer )
    GOM.RING.DFGM-RAW GOM.RING.FETCH
;

: DFGM.RAW.TAIL-INC ( n -- , Increment tail n times )
    GOM.RING.DFGM-RAW -1 1 RING.MOVE
;

: DFGM.RAW.TAIL-DEC ( n -- , Increment tail n times )
    GOM.RING.DFGM-RAW -1 -1 RING.MOVE
;

: DFGM.RAW.HEAD-INC ( n -- , Increment tail n times )
    GOM.RING.DFGM-RAW 1 1 RING.MOVE
;

: DFGM.RAW.HEAD-DEC ( n -- , Increment tail n times )
    GOM.RING.DFGM-RAW 1 -1 RING.MOVE
;    

: DFGM.S0.DOWNLOAD ( n --, Download n stream 0 DFGM files )
  	GOM.RING.DFGM-S0 RING.DOWNLOAD
;


: DFGM.S0.FETCH ( -- , Fetch head and tail of DFGM S0 ring buffer )
    GOM.RING.DFGM-S0 GOM.RING.FETCH
;

: DFGM.S1.DOWNLOAD ( n --, Download n stream 1 DFGM files )
  	GOM.RING.DFGM-S1 RING.DOWNLOAD
;

: DFGM.S1.FETCH ( -- , Fetch head and tail of DFGM S1 ring buffer )
    GOM.RING.DFGM-S1 GOM.RING.FETCH
;

: DFGM.DOWNLOAD+ ( n -- n , Fetch ring tails then download n DFGM files. Puts error code of fetch on the stack )
    	GOM.RING.FETCH-ALL-TAILS DUP
	GOM.ERR.OK = NOT IF
		." Error fetching tails"
	ELSE
		DROP
		DFGM.RAW.DOWNLOAD
	THEN
;

: Tyler ( --, downloads specific DFGM file )
    CR ." Salmon!" CR
	S" ftp download_file /sd/004R0004.bin" GOM
;

: Dustin ( --, downloads specific DFGM file )
    CR ." CHARIZARD GO!" CR
	S" ftp download_file /sd/146R0326.bin" GOM
;		

: Drew ( --, downloads specific DFGM file )
	S" ftp download_file /sd/001R0001.bin" GOM
;

: DUSTIN.1 ( --, downloads specific DFGM file )
	S" ftp download_file /sd/002R0002.bin" GOM
;

: DUSTIN.2 ( --, downloads specific DFGM file )
	S" ftp download_file /sd/003R0003.bin" GOM
;	

: DUSTIN.3 ( --, downloads specific DFGM file )
	S" ftp download_file /sd/004R0004.bin" GOM
;	

: DUSTIN.4 ( --, downloads specific DFGM file )
	S" ftp download_file /sd/005R0005.bin" GOM
;			

\ ******************************************************************************\
\ OBC words									\
\ ******************************************************************************\
: OBC.REBOOT ( -- , Soft reboot OBC in real time )
	S" reboot 1" GOM
;

: RTC.UPDATE ( -- , pulls unix time from groundstation server clock and updates OBC RTC )
	S" obc ts 1" GOM
;

: RTC.FETCH ( -- , polls unix time from the nanomind RTC. )
	S" obc ts 0" GOM
;


: OBC.HK.DOWNLOAD ( -- , downloads the 4 obc hk files from /boot/. Requires interaction )
	S" ftp backend 2" GOM
	WAIT
	S" ftp download_file /boot/state_log.bin" GOM
	WAIT
	S" ftp download_file /boot/tc_log.bin" GOM
	WAIT
	S" ftp download_file /boot/bootcause.bin" GOM
	WAIT
	S" ftp download_file /boot/state_log.bin" GOM
;

: OBC.HK.CLEAN ( -- , deletes the 4 obc hk files from /boot/. Requires interaction )
	S" ftp backend 2" GOM
	WAIT
	S" ftp rm /boot/state_log.bin" GOM
	WAIT
	S" ftp rm /boot/tc_log.bin" GOM
	WAIT
	S" ftp rm /boot/bootcause.bin" GOM
	WAIT
	S" ftp rm /boot/state_log.bin" GOM
;


\ ******************************************************************************\
\ Athena words									\
\ ******************************************************************************\
: ATHENA.DOWNLOAD ( n -- , Download n Athena files )
  	GOM.RING.ATHENA RING.DOWNLOAD
;
