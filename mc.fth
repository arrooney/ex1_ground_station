\ ******************************************************************************\
\ Thresholds Words								\
\ ******************************************************************************\
16100 CONSTANT EPS-MAX-VBATT
14000 CONSTANT EPS-MIN-VBATT
0 CONSTANT EPS-MIN-BATT-TEMP
30 CONSTANT OBC-MAX-BOOTCOUNT

\ ******************************************************************************\
\ Helper Words									\
\ ******************************************************************************\
: MC.ALL-CLEAR ( -- , Prints the string ALL CLEAR in green )
    S" ALL CLEAR" 3 TYPE.COLOR
;

: MC.WARNING ( -- , Prints the string WARNING in yellow )
    S" WARNING" 2 TYPE.COLOR
;

: MC.ERROR ( -- , Prings the string ERROR in magenta )
    S" ERROR" 5 TYPE.COLOR
;

: MC.CRITICAL ( -- , Prings the string CRITICAL ERROR in red )
    S" CRITICAL SAFETY ANOMOLY" 1 TYPE.COLOR
;

: TIME+ ( n -- q, q = current_unix_time + n )
    TIME +
;

: MC.AUTO ( a0, a1, ..., a[b-1], xt, b, n -- q,
    n = unix time of expected LOS
    xt = execution token for any word the pushes a GOM.ERR code on the stack
    a0, ... a[b-1] = arguments for the execution token xt
    b = number of arguments
    q = error code:
    	-1 TRUE => xt word was succesful, ie, pushed GOM.ERR.OK, before expected LOS
    	0 FALSE => Could not get successful execution before expected LOS
    Executes xt with all it's arguments every 2 seconds until it pushes
    TRUE on the stack or expected LOS is reached. )
;

: MC.VERIFY ( n, xt -- q
    n = unix time of expected LOS
    xt = execution token for any word the pushes a GOM.ERR code on the stack
    q = error code:
    	-1 TRUE => xt word was succesful, ie, pushed GOM.ERR.OK, before expected LOS
    	0 FALSE => Could not get successful execution before expected LOS
    Executes xt every 2 seconds until it pushes TRUE on the stack or expected LOS is reached. )
    BEGIN
	DUP EXECUTE
	."  - timestamp: " TYPE.TIME CR	
	GOM.ERR.OK = NOT IF

	    \ Did not get succesful execution, check if we're at expected LOS
	    OVER TIME < IF
		\ Past expected LOS, failed to execute xt 
		DROP DROP FALSE TRUE
	    ELSE
		\ Wait 2 seconds before next attempt
		2 SLEEP FALSE
	    THEN

	ELSE
	    
	    \ Got successful execution
	    DROP DROP TRUE TRUE
	    
	THEN
    UNTIL
;


\ ******************************************************************************\
\ AOS 										\
\ ******************************************************************************\
: AOS.PING ( addr, n1, n2 -- n3, Input is short description of system being pinged
    				 n2 is node being pinged
    				 Output is GOM.ERR code from the ping
    				 NOTE: this word shouldn't be called from the command line )
    1000 10 GOM.PING
    DUP GOM.ERR.OK = NOT IF
	\ Ping failed
	MC.WARNING CR
	2 PICK 2 PICK TYPE
	S" : Did not respond to ping" TYPE
    ELSE
	MC.ALL-CLEAR
    THEN

    \ Clean up the stack
    ROT ROT DROP DROP
;

: AOS.GOMSHELL ( -- n, puts a GOM.ERR code on stack from AOS attempt )
    S" node 10 - gomshell loop back" 10 AOS.PING
;
    
: AOS.RADIO ( -- n, puts a GOM.ERR code on stack from AOS attempt )
    S" node 15 - ground station radio" 15 AOS.PING
;

: AOS.NANOCOM ( -- n, puts a GOM.ERR code on stack from nanocom AOS attempt )
    S" node 5 - nanocom" 5 AOS.PING
;

: AOS.NANOHUB ( -- n, puts a GOM.ERR code on stack from AOS attempt )
    S" node 3 - nanohub" 3 AOS.PING
;

: AOS.NANOPOWER ( -- n, puts a GOM.ERR code on stack from AOS attempt )
    S" node 2 - nanopower" 2 AOS.PING
;

: AOS.NANOMIND ( -- n, puts a GOM.ERR code on stack from AOS attempt )
    S" node 1 - nanomind" 1 AOS.PING
;

\ ******************************************************************************\
\ Nanocom Health Check								\
\ ******************************************************************************\
: COM.VERIFY ( n -- q,
    n = unix time of expected LOS
    q = error code.
	TRUE => COM is healthy and AOS complete
	FALSE => Could not get AOS before expected LOS
    Verify the data link with the nanocom )
    
    ['] AOS.NANOCOM
    MC.VERIFY
;

: COM.VERIFY+ ( n -- q,
    n = number of seconds to perform COM.VERIFY for
    q = same error code as COM.VERIFY )

    TIME+ COM.VERIFY
;

\ ******************************************************************************\
\ OBC Health Check								\
\ ******************************************************************************\
: OBC.BOOTCOUNT ( -- q, q = current boot count )
    GOM.OBC.BOOT-STATE
;

: OBC.VERIFY ( n -- q,
    n = unix time of expected LOS
    q = error code, same meaning as COM.VERIFY
    Verify data link with OBC )

    ['] AOS.NANOMIND
    MC.VERIFY
;

: OBC.VERIFY+ ( n -- q,
    n = number of seconds to perform OBC.VERIFY for
    q = error code. Same as OBC.VERIFY )
    TIME+ OBC.VERIFY
;

: OBC.HEALTH-CHECK ( n -- q1, q2,
    n = unix time of expected LOS
    q1 = current OBC boot count. Only valid if q2 = {-1, -2}
    q2 = error code
	-1 = health check passed. OBC is happy.
    	-2 = AOS, but health check failed. OBC is unhappy.
    	-3 = AOS, but could not get HK to perform health check
    	-4 = Could not get AOS before LOS
    Perform a health check on the OBC using it's real-time telemetry )
    DUP 
    OBC.VERIFY FALSE = IF
	\ Coult not get AOS
	DROP -4 EXIT
    THEN

    BEGIN
	OBC.BOOTCOUNT
	."  - timestamp: " TYPE.TIME CR	
	DUP 0 < IF
	   
	    \ Did not get succesful execution, check if we're at expected LOS
	    DROP DUP TIME < IF
		\ Past expected LOS, failed to execute xt 
		DROP -1 -3 EXIT
	    ELSE
		\ reattempt 
		FALSE
	    THEN

	ELSE
	    \ Got successful execution
	    SWAP DROP TRUE
	THEN
    UNTIL

    DUP OBC-MAX-BOOTCOUNT > IF
	-2
    ELSE
	-1
    THEN
;

\ ******************************************************************************\
\ EPS Health Check								\
\ ******************************************************************************\
: EPS.CACHE ( -- q, Caches real time eps hk. q is a GOM.ERR code )
    GOM.EPS.GET-HK
;

: EPS.VERIFY ( n -- q,
    Same as OBC.VERIFY and COM.VERIFY, but for the EPS system )

    ['] AOS.NANOPOWER
    MC.VERIFY
;

: EPS.VERIFY+ ( n -- q,
    Same as OBC.VERIFY+ and COM.VERIFY+, but for the EPS system )
    TIME+ EPS.VERIFY
;

: EPS.VBOOST[n] ( n -- q, q = vboost[n] )
    1 SWAP GOM.EPS.INDEX-HK
;

: EPS.VBATT ( -- q, q = vbatt )
    2 0 GOM.EPS.INDEX-HK
;

: EPS.CURIN[n] ( n -- q, q = curin[n] )
    3 SWAP GOM.EPS.INDEX-HK
;

: EPS.CURSUN ( -- q, q = cursun )
    4 0 GOM.EPS.INDEX-HK
;

: EPS.CURSYS ( -- q, q = cursys )
    5 0 GOM.EPS.INDEX-HK
;

: EPS.CUROUT[n] ( n -- q, q = curout[n] )
    7 SWAP GOM.EPS.INDEX-HK
;

: EPS.OUTPUT[n] ( n -- q, q = output[n] )
    8 SWAP GOM.EPS.INDEX-HK
;

: EPS.OUTPUT-ON-DELTA[n] ( n -- q, q = output_on_delta[n] )
    9 SWAP GOM.EPS.INDEX-HK
;

: EPS.OUTPUT-OFF-DELTA[n] ( n -- q, q = output_off_delta[n] )
    10 SWAP GOM.EPS.INDEX-HK
;

: EPS.LATCHUP[n] ( n -- q, q = latchup[n] )
    11 SWAP GOM.EPS.INDEX-HK
;

: EPS.WDT-I2C-TIME-LEFT ( -- q, q = wdt_i2c_time_left )
    12 0 GOM.EPS.INDEX-HK
;

: EPS.WDT-GND-TIME-LEFT ( -- q, q = wdt_gnd_time_left )
    13 0 GOM.EPS.INDEX-HK
;

: EPS.WDT-CSP-OBC-PINGS-LEFT ( -- q, q =  wdt_csp_pings_left[0] )
    14 0 GOM.EPS.INDEX-HK
;

: EPS.WDT-CSP-COM-PINGS-LEFT ( -- q, q = wdt_csp_pings_left[1] )
    14 1 GOM.EPS.INDEX-HK
;

: EPS.COUNTER-WDT-I2C ( -- q, q = counter_wdt_i2c )
    15 0 GOM.EPS.INDEX-HK
;

: EPS.COUNTER-WDT-GND ( -- q, q = counter_wdt_gnd )
    16 0 GOM.EPS.INDEX-HK
;

: EPS.COUNTER-WDT-CSP-OBC ( n -- q, q = counter_wdt_csp[0] )
    17 0 GOM.EPS.INDEX-HK
;

: EPS.COUNTER-WDT-CSP-COM ( n -- q, q = counter_wdt_csp[1] )
    17 1 GOM.EPS.INDEX-HK
;

: EPS.COUNTER-BOOT ( -- q, q = counter_boot )
    18 0 GOM.EPS.INDEX-HK
;

: EPS.TEMP[n] ( n -- q, q = temp[n] )
    19 SWAP GOM.EPS.INDEX-HK
;

: EPS.BOOTCAUSE ( -- q, q = bootcause )
    20 0 GOM.EPS.INDEX-HK
;

: EPS.BATTMODE ( -- q, q = battmode )
    21 0 GOM.EPS.INDEX-HK
;

: EPS.PPTMODE ( -- q, q = pptmode )
    22 0 GOM.EPS.INDEX-HK
;

: EPS.HEALTH-CHECK ( n -- q,
    n = Expected unix time of LOS
    q = error code
	    -1 = health check passed. EPS is happy.
	    -2 = AOS, but health check failed. EPS is unhappy.
	    -3 = Could not get AOS before LOS
    Perform a health check on the EPS using it's real-time telemetry )
    DUP 
    EPS.VERIFY FALSE = IF
	\ Coult not get AOS
	DROP -3 EXIT
    THEN

    DUP ['] EPS.CACHE
    MC.VERIFY FALSE = IF
	\ Could not cache real-time hk before LOS
	DROP -3 EXIT
    THEN

    -1 \ return code. Will get change if threshold testing fails.
    EPS.VBATT EPS-MIN-VBATT <
    EPS.VBATT EPS-MAX-VBATT >
    OR IF
	DROP -2 \ Change return code
    THEN

    4 EPS.TEMP[N] EPS-MIN-BATT-TEMP <
    5 EPS.TEMP[N] EPS-MIN-BATT-TEMP <
    OR IF
	-1 = IF \ Change return code if VBATT test hasn't failed and changed it.
	    DROP -2
	THEN
    THEN
;


\ ******************************************************************************\
\ Thresholds Words								\
\ ******************************************************************************\
: MC.SYSTEM-CHECK ( n -- q, Perform S/C wide health check
    n = LOS unix time
    q = Error code
	    -1 => all systems go
	    -2 => error encountered
	    -3 => anomoly encountered )
    \ Nanocom link verification
    DUP COM.VERIFY
    FALSE = IF
	MC.ERROR ."  Could not establish link with nanocom. time: " TYPE.TIME CR
	-2 EXIT
    THEN

    \ EPS link verification and health check
    DUP EPS.HEALTH-CHECK
    DUP -3 = IF
	MC.ERROR ."  Could not establish link with nanopower. time: " TYPE.TIME CR
	-2 EXIT
    THEN

    ." Current EPS.HK: " CR
    GOM.EPS.PRINT
    
    -2 = IF
	MC.CRITICAL ."  Nanopower's telemetry indicates S/C safety is in jeopardy."
	MC.CRITICAL ."  time: " TYPE.TIME
	-3 EXIT
    THEN

    \ OBC link  verification and health check
    OBC.HEALTH-CHECK
    DUP -4 = IF
	MC.ERROR ."  Could not establish link with nanomind. time: " TYPE.TIME CR
	-2 EXIT
    THEN

    DUP -3 = IF
	MC.ERROR ."  Failed to retrieve nanomind's housekeeping. time: " TYPE.TIME CR
	-2 EXIT
    THEN

    SWAP ." Current bootcount of nanomind: " . CR
    -2 = IF
	MC.CRITICAL ."  Nanomind is safemode. S/C safety may be in jeopardy." CR
	MC.CRITICAL ."  time: " TYPE.TIME
	-3 EXIT
    THEN
;