
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
	S" eps hk" GOM
;

: DOWN ( -- , downloads the file of the night from SD card only )
	S" ftp download_file /boot/adcs.bin" GOM
;

: BRENDAN
	S" /sd/MT-RLog.txt" GOM.FTP.DOWNLOAD WAIT
	S" /sd/MT-ALog.txt" GOM.FTP.DOWNLOAD WAIT
	S" /sd/MT-BLog.txt" GOM.FTP.DOWNLOAD WAIT
	S" /sd/MT-HLog.txt" GOM.FTP.DOWNLOAD WAIT
;

: DOWN&RM ( -- , downloads the file and then deletes after with user input)
	S" ftp download_file /sd/020W0272.bin" GOM
	WAIT
	KEY
	[CHAR] d = IF
		." Removing" CR
		S" ftp rm /sd/020W0272.bin" GOM
	THEN
;

: RM ( -- , remove the file of the night. )
	S" ftp rm /boot/tc_log.bin" GOM
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

: ADCS.RTC.GET ( -- , gets the clock from the ADCS OBC RTC )
	S" obc adcs 143 6 null.txt" GOM
;

: ADCS.CRC ( -- , polls telemetry 241 to reset the file download to nanomind )
	S" obc adcs 241 6 null.txt" GOM
;

: ADCS.FORMAT.FS ( -- , delete all files in the ADCS sd card! )
	S" obc adcs 119 0 null.txt" GOM
;

: ADCS.CSS ( -- , gets the raw coarse sun sensor values from the ADCS )
	S" obc adcs 166 6 null.txt" GOM
	WAIT
	S" obc adcs 167 6 null.txt" GOM
;

: ADCS.DEPLOY ( -- , attempts mag deployment with timeout in txt file. )
	S" obc adcs 6 1 deploy_mag_20.txt" GOM
;

: ADCS.SET.RTC ( -- , updates the clock with computer time)
	S" obc adcs 2 6 null.txt" GOM
;

: ADCS.SAVE.CONFIG ( -- , writes config from ram to boot file )
	S" obc adcs 100 0 null.txt" GOM
;

: ADCS.SET.NADIR ( -- , uploads nadir camera settings )
	S" obc adcs 85 57 nadir_eauto.txt" GOM
;

: ADCS.SET.TORQ ( -- , uploads magnetotorquer settings )
	S" obc adcs 81 13 torq_config.txt" GOM
;

: ADCS.SET.WHEEL ( -- , uploads wheel settings )
	S" obc adcs 82 13 wheel_config.txt" GOM
;

: ADCS.SET.MAG ( -- , uploads mag settings )
	S" obc adcs 86 30 deployed_mag_uncal.txt" GOM
;

: ADCS.SET.CSS ( -- , uploads CSS settings )
	S" obc adcs 83 14 css_config.txt" GOM
;

: ADCS.SET.RATESENSOR ( -- , uploads rate sensor settings )
	S" obc adcs 87 6 rot_sens_config.txt" GOM
;

: ADCS.SET.TLE ( -- , uploads TLE to adcs )
	S" obc adcs 64 64 tle_adcs_jul1.txt" GOM
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

: ADCS.TAKE.PHOTO ( -- , take photo. Currently nadir camera. )
	S" obc adcs 110 10 take_photo_nadir.txt" GOM
	WAIT
	s" obc adcs 230 16 null.txt" GOM
	WAIT
;

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

: RING.FETCH ( -- , run once per pass to populate ring tails for downloading in all rings )
	GOM.RING.FETCH DUP
	GOM.ERR.OK = NOT IF
		." Error fetching tails"
	ELSE
		DROP
		." Fetch successful"
	THEN
;


\ ******************************************************************************\
\ WOD words									\
\ ******************************************************************************\
: WOD.DOWNLOAD ( n -- , Download n WOD files )
        GOM.RING.WOD RING.DOWNLOAD
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

: DFGM.MODE.RAW ( -- , Enable raw capture for DFGM )
	S" dfgm stream|raw" GOM.COMMAND
	CHECK-ERR
;

: DFGM.RAW.DOWNLOAD ( n -- , Download n raw DFGM files )
	GOM.RING.DFGM-RAW RING.DOWNLOAD
;

: DFGM.S0.DOWNLOAD ( n --, Download n stream 0 DFGM files )
  	GOM.RING.DFGM-S0 RING.DOWNLOAD
;

: Tyler ( --, downloads specific DFGM file )
	S" ftp download_file /sd/05fR0095.bin" GOM
;

: DFGM.S1.DOWNLOAD ( n --, Download n stream 1 DFGM files )
  	GOM.RING.DFGM-S1 RING.DOWNLOAD
;

: DFGM.DOWNLOAD+ ( n -- n , Fetch ring tails then download n DFGM files. Puts error code of fetch on the stack )
	GOM.RING.FETCH DUP
	GOM.ERR.OK = NOT IF
		." Error fetching tails"
	ELSE
		DROP
		DFGM.RAW.DOWNLOAD
	THEN
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
