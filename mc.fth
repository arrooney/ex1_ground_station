\ ******************************************************************************\
\ Helper Words									\
\ ******************************************************************************\
: MC.ALL-CLEAR ( -- , Prints the string ALL CLEAR in green )
    S" ALL CLEAR" 3 TYPE.COLOR
;

: MC.WARNING ( -- , Prints the string WARNING in yellow )
    S" WARNING" 2 TYPE.COLOR
;

\ ******************************************************************************\
\ AOS 										\
\ ******************************************************************************\
: AOS.PING ( addr, n1, n2 -- n3, Input is short description of system being pinged
    				n2 is node being pinged
    			Output is GOM.ERR code from the ping )
    1000 10 GOM.PING
    DUP GOM.ERR.OK = NOT IF
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