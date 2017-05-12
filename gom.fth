: CHECK-ERR
	GOM.ERR.OK = NOT
	IF
		CR ." gom error" CR
	THEN
;

: LS-SD S" ftp ls /sd/" GOM ;
: LS-BOOT S" ftp ls /boot/" GOM ;

: DFGM-DIAG S" dfgm diag" GOM.COMMAND CHECK-ERR ;
: DFGM-ON S" dfgm power|on" GOM.COMMAND CHECK-ERR ;
: DFGM-OFF S" dfgm power|off" GOM.COMMAND CHECK-ERR ;

: REBOOT S" reboot 1" GOM ;

: RING.DOWNLOAD
	SWAP 0 DO
		DUP GOM.RING.DOWNLOAD
		GOM.ERR.OK = NOT IF
			." Error downloading at index: " I . CR
			." Stack trace: " .S CR
		THEN
	LOOP
	." Done"
;

: DFGM.DOWNLOAD
	GOM.RING.DFGM-RAW RING.DOWNLOAD
;

: DFGM.DOWNLOAD+
	GOM.RING.FETCH
	GOM.ERR.OK = NOT IF
		." Error fetching tails"
		QUIT
	THEN

	DFGM.DOWNLOAD
;		

