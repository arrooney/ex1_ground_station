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
	GOM.RING.FETCH
	GOM.ERR.OK = NOT IF
		." Error fetching tails"
		QUIT
	THEN

	1 DO
		GOM.RING.WOD GOM.RING.DOWNLOAD
		GOM.ERR.OK = NOT IF
			." Error downloading at index: " I . CR
		THEN
	LOOP	
;
