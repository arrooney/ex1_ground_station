include strings.fth

\ Example usage of dispatch.fth
\ Assume the following script:
\	WAIT
\	S" ftp ls /boot/" GOM
\	WAIT
\	S" eps hk get" GOM
\ Then, the user inputs the following character sequences at each WAIT
\	r
\	n
\	r
\	r
\	n
\ The gomshell commands run will be:
\	*nothing*
\	ftp ls /boot/
\	ftp ls /boot/
\	ftp ls /boot/
\	eps hk get


\ Allocate space for a 64 character command string
64 $VAR gom-history

\ Note, this word must be compiled before the redefinition of
\ GOM
: DISPATCH-EXECUTE ( -- , Word the executes a gomshell command )
    GOM
;

\ Execute the commnand string on the stack and save it to the histroy
\ buffer.
\ Example usage:
\ 	S" ftp ls /boot/" DISPATCH
\ This will list the contents of the boot directory.
: GOM ( addr count -- )
    \ Save the command in the history buffer
    gom-history $!
    gom-history DISPATCH-EXECUTE
;

\ Blocks until the user enters a character
\ The character:effects pairs are:
\ q: calls QUIT, returns user to forth prompt
\ r: executes the previous gomshell command
\ anything else: finishes execution of the word, does nothing else.
: WAIT ( -- )
    BEGIN
	\ Wait for user to input a char
	KEY DUP

	\ If they entered 'q' for quit.
	[CHAR] q = IF 
	    DROP ." Quitting" CR
	    QUIT
	THEN

	\ If they entered 'r' for rewind
	[CHAR] r = IF
	    CR ." rewind:" gom-history DISPATCH-EXECUTE
	    FALSE \ Continue loop

	\ User wishes to continue - ie, exit this word.
	ELSE
	    TRUE \ End loop
	THEN
    UNTIL
;

	    
	