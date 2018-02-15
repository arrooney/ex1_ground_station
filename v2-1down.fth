INCLUDE mc.fth
INCLUDE gom.fth

0 CONSTANT AOS
10 60 * TIME+ CONSTANT LOS

: v2-1down
    LOS COM.VERIFY
    FALSE = IF
	EXIT
    THEN

    LOS EPS.HEALTH-CHECK
    -1 = NOT IF
	EXIT
    THEN
    ." EPS Health check passed " MC.ALL-CLEAR CR
    ." VBATT = " EPS.VBATT . CR
    ." EPS.HK for your viewing pleasure" CR EPS.HK

    LOS OBC.HEALTH-CHECK
    -1 = NOT IF
	EXIT
    THEN
    ." OBC Health check passed " MC.ALL-CLEAR CR

    \ All systems healthy, download the file
    BEGIN
	." Download telecommand sent at: " TYPE.TIME CR	
	S" /boot/nanominddec12.lzo" GOM.FTP.DOWNLOAD
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

v2-1down


    
    