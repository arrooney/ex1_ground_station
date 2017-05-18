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
: DFGM-DIAG ( -- n , Print real time diagnostics about dfgm. Puts error code on stack )
	S" dfgm diag" GOM.COMMAND DUP
	CHECK-ERR
;

: DFGM-ON ( -- n , Turn on dfgm in real time. Puts error code on stack )
	S" dfgm power|on" GOM.COMMAND
	CHECK-ERR
;

: DFGM-OFF ( -- n , Turn off dfgm in real time. Puts error code on stack )
	S" dfgm power|off" GOM.COMMAND
	CHECK-ERR
;

: DFGM.RAW.DOWNLOAD ( n -- , Download n raw DFGM files )
	GOM.RING.DFGM-RAW RING.DOWNLOAD
;

: DFGM.S0.DOWNLOAD ( n --, Download n stream 0 DFGM files )
  	GOM.RING.DFGM-S0 RING.DOWNLOAD
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


\ ******************************************************************************\
\ Athena words									\
\ ******************************************************************************\
: ATHENA.DOWNLOAD ( n -- , Download n Athena files )
  	GOM.RING.ATHENA RING.DOWNLOAD
;