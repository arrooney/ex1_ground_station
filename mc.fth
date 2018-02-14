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
    S" CRITICAL ERROR" 1 TYPE.COLOR
;

: TIME+ ( n -- q, q = current_unix_time + n )
    TIME +
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

: AOS.VERIFY ( n, xt -- q
    n = unix time of expected LOS
    xt = execution token for AOS.XXX word above
    q = error code:
    	-1 TRUE => AOS was verified healthy
    	0 FALSE => Could not get AOS before expected LOS
    Waits until AOS of system or expected LOS, which ever comes first. )
    BEGIN
	DUP EXECUTE
	."  - timestamp: " TYPE.TIME CR
	GOM.ERR.OK = IF
	    \ Got AOS
	    DROP DROP TRUE LEAVE
	THEN

	\ Did not get AOS, check if we're at expected LOS
	OVER TIME < IF
	    \ Past expected LOS, failed to get AOS
	    CR MC.CRITICAL
	    CR ." Failed to get Nanocom's AOS before expected LOS"
	    DROP DROP FALSE LEAVE
	THEN

	\ Wait 2 seconds before next attempt
	2 SLEEP
    FALSE UNTIL
;

\ ******************************************************************************\
\ Nanocom Health Check								\
\ ******************************************************************************\
: COM.VERIFY ( n -- q,
    n = unix time of expected LOS
    q = error code.
	TRUE => COM is health and AOS complete
	FALSE => Could not get AOS before expected LOS
    Verify the data link with the nanocom )
    
    ['] AOS.NANOCOM
    AOS.VERIFY
;

: COM.VERIFY+ ( n -- q,
    n = number of seconds to perform COM.VERIFY for
    q = same error code as COM.VERIFY )

    TIME+ COM.VERIFY
;

\ ******************************************************************************\
\ OBC Health Check								\
\ ******************************************************************************\
: OBC.BOOTCOUNT ( -- q, Caches obc bootcause. q is the current boot number )
    GOM.OBC.BOOT-STATE
    DUP 0 < IF
	CR MC.WARNING
	CR ." Failed to cache nanomind's boot cause"
    THEN
;

: OBC.VERIFY ( n -- q,
    n = unix time of expected LOW
    q = error code, same meaning as COM.VERIFY
    Verify data link with OBC )

    ['] AOS.NANOMIND
    AOS.VERIFY
;

: OBC.VERIFY+ ( n -- q,
    n = number of seconds to perform OBC.VERIFY for
    q = error code. Same as OBC.VERIFY )
    TIME+ OBC.VERIFY
;

\ ******************************************************************************\
\ EPS Health Check								\
\ ******************************************************************************\
: EPS.CACHE ( -- q, Caches real time eps hk. q is GOM.ERR error code )
    GOM.EPS.GET-HK
    DUP GOM.ERR.OK = NOT IF
	CR MC.WARNING
	CR ." Failed to cache nanopower's real-time HK with error: "
	DUP .
    THEN
;

: EPS.VERIFY ( n -- q,
    Same as OBC.VERIFY and COM.VERIFY, but for the EPS system )

    ['] AOS.NANOPOWER
    AOS.VERIFY
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

