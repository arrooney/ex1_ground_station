INCLUDE gom.fth

: BOOT.WAIT
    S" Continue when boot is finished [y/q]" TYPE
    WAIT
;

: TEST.REBOOT
    S" Rebooting" TYPE
    S" reboot 1" GOM
    BOOT.WAIT
;

40 $VAR rb.str
: TEST.RBCONF
    S" Use [y/q] when prompted for test success / failure" TYPE
    TEST.RBCONF-WOD
    TEST.RBCONF-DFGM
;

: TEST.RBCONF-WOD
    S" wod" rb.str $!
    
    S" Running ring buffer configuration tests for: " TYPE rb.str TYPE CR
    S" Decreasing RB capacity and size" TYPE
    rb.str 3 266 RING.RESIZE

    RING.CURRENT
    S" Verify RB has capacity=3 file_size=266" TYPE
    WAIT
    
    RING.FLUSH
    S" Verify flush was successful" TYPE
    WAIT
    
    S" Fetching current head/tail then rebooting" GOM
    WOD.FETCH
    GOM.ERR.OK = NOT IF
	S" Fetch failed" TYPE
	QUIT
    THEN
    TEST.REBOOT

    S" Wait for 7 beacons then continue [y]" TYPE
    WAIT
    S" Verify head/tail [y/q]" TYPE
    WOD.FETCH
    GOM.ERR.OK = NOT IF
	S" Fetch failed" TYPE
	QUIT
    THEN
    WAIT

    TEST.REBOOT
    S" Verify ring buffer sizes are their default [y/q]" TYPE
    RING.CURRENT
    WAIT

    rb.str 3 266 RING.RESIZE
    S" Verify set" TYPE
    WAIT
    RING.FLUSH
    S" Verify flush [y/q]" TYPE
    WAIT
    RING.COMMIT
    S" Verify commit [y/q]" TYPE
    WAIT
    
    TEST.REBOOT
    RING.CURRENT
    S" Verify wod capacity=3 and file_size=266 [y/q]" TYPE
    WAIT
    TEST.REBOOT
    S" Verify wod capacity=3 and file_size=266 [y/q]" TYPE
    WAIT
    S" WOD test finished" TYPE
;

: TEST.RBCONF-DFGM
    S" dfgm raw" rb.str $!
    
    S" Running ring buffer configuration tests for: " TYPE rb.str TYPE CR
    S" Decreasing RB capacity and size" TYPE
    rb.str 3 6244 RING.RESIZE

    RING.CURRENT
    S" Verify RB has capacity=3 file_size=6244" TYPE
    WAIT
    
    RING.FLUSH
    S" Verify flush was successful" TYPE
    WAIT
    
    S" Fetching current head/tail then rebooting" GOM
    DFGM.RAW.FETCH
    GOM.ERR.OK = NOT IF
	S" Fetch failed" TYPE
	QUIT
    THEN
    TEST.REBOOT

    S" Turning DFGM on" TYPE
    20 SLEEP
    S" Verify head/tail [y/q]" TYPE
    DFGM.RAW.FETCH
    GOM.ERR.OK = NOT IF
	S" Fetch failed" TYPE
	QUIT
    THEN
    WAIT

    TEST.REBOOT
    S" Verify ring buffer sizes are their default [y/q]" TYPE
    RING.CURRENT
    WAIT

    rb.str 3 6244 RING.RESIZE
    S" Verify set" TYPE
    WAIT
    RING.FLUSH
    S" Verify flush [y/q]" TYPE
    WAIT
    RING.COMMIT
    S" Verify commit [y/q]" TYPE
    WAIT
    
    TEST.REBOOT
    RING.CURRENT
    S" Verify capacity=3 and file_size=6244 [y/q]" TYPE
    WAIT
    TEST.REBOOT
    S" Verify wod capacity=3 and file_size=6244 [y/q]" TYPE
    WAIT
    S" DFGM test finished" TYPE
;
