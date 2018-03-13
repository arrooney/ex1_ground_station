INCLUDE mc.fth

\ First, set the AOS and LOS of the pass.
\ The AOS is the unix time when the pass starts
\ The LOS is the unix time when the pass ends.
\ For example, this sets up a pass with a start time of 15 seconds after
\ unix epoch and ends 189 seconds after the unix epoch.
\ 15 189 PASS.SET-TIME
7 TIME+ 60 TIME+ PASS.SET-TIME

\ Wait for the pass to start
PASS.WAIT-FOR-AOS

\ Perform a check on all S/C systems
PASS.SYSTEM-CHECK

\ Check the status of the system check. If it failed
\ the script will end here. If it passes, the script
\ will continue to download WOD.
-1 = NOT IF
    EXIT
THEN

\ Download 3 WOD files
1 PASS.WOD