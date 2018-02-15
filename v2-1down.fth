INCLUDE mc.fth
INCLUDE gom.fth

0 CONSTANT AOS
10 60 * TIME+ CONSTANT LOS

: v2-1down
    \ Perform health check on all systems
    LOS MC.SYSTEM-CHECK
    -1 = NOT IF
	EXIT
    THEN
    
    \ All systems healthy, download the file
    ." All systems are clear for operation" CR
    BEGIN
	." Download telecommand sent at: " TYPE.TIME CR	
	S" /boot/nanominddec12.bin" GOM.FTP.DOWNLOAD
	GOM.ERR.OK = NOT IF
	    \ Did not get succesful execution, check if we're at expected LOS
	    LOS TIME < IF
		\ Past expected LOS, failed to execute xt 
		CR MC.WARNING
		CR ." Failed to finish download before LOS"
		EXIT
	    ELSE
		\ Not past LOS, continue.
		FALSE
	    THEN
	ELSE
	    \ Got successful execution
	    TRUE
	THEN
    UNTIL

    CR ." v2-1 down complete"
;




    
    