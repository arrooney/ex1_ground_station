
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
	S" ftp download_file /sd/170R0368.bin" GOM
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

: RM ( -- , remove the file of the night. )
	S" ftp rm /sd/009R0009.bin" GOM
;

: ADCS.RESET ( -- , resets the adcs. Wait 15 seconds for it to boot. )
	S" obc adcs 1 1 reset.txt" GOM
;

: ADCS.CRC ( -- , polls telemetry 241 )
	S" obc adcs 241 6 null.txt" GOM
;

: ADCS.FORMAT.FS ( -- , delete all files in the ADCS sd card! )
	S" obc adcs 119 0 null.txt" GOM
;

: ADCS.CSS ( -- , gets the raw coarse sun sensor values from the ADCS )
	S" obc adcs 166 6 null.txt" GOM
;

: ADCS.SAVE.CONFIG ( -- , writes config from ram to boot file )
	S" obc adcs 100 0 null.txt" GOM
;

: ADCS.SET.NADIR ( -- , uploads nadir camera settings )
	S" obc adcs 85 57 nadir_e1.txt" GOM
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

: DFGM.MODE.RAW ( -- , Enable raw capture for DFGM )
	S" dfgm stream|raw" GOM.COMMAND
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
    GOM.RING.DFGM-RAW GOM.RING.FETCH
;

: DFGM.S1.DOWNLOAD ( n --, Download n stream 1 DFGM files )
  	GOM.RING.DFGM-S1 RING.DOWNLOAD
;

: DFGM.S1.FETCH ( -- , Fetch head and tail of DFGM S1 ring buffer )
    GOM.RING.DFGM-RAW GOM.RING.FETCH
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
