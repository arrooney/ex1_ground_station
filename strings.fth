\ ******************************************************************************\
\ String manipulation words							\
\ ******************************************************************************\
\ The words defined here only work on string variables created using the word
\ $VAR. These words will corrupt your memory if used on any other data structure.
\ Some exceptions exist. Any word that references a "source string / src-addr"
\ like $SET, $APPEND, and $PREFIX will work when the source string is not
\ a $VAR.

\ Example usage: Creating two strings and appending one to the other
\ 20 $VAR hello-str	      	\ Create a variable, hello-str
\ 20 $VAR world-str	      	\ Create a variable, world-str
\
\ S" hello" hello-str $SET	\ Move the string "hello" into the variable
\ S" world" world-str $SET	\ Move the string "world" into the variable
\
\ hello-str world-str $APPEND	\ Append the contents of world-str to hello-str
\
\ hello-str CR TYPE		\ Print the contents of hello-str
\ helloworld   			\ The result of printing 

\ $VAR YYY ( n -- , Create an n byte character string by the name of YYY )
\ YYY: ( -- addr count, Puts the string address and current length on the stack )
: $VAR
	\ Create dictionary entry YYY
	CREATE

	\ Compile n into dictionary, the current string length, followed by n empty bytes
	DUP , 0 , ALLOT

	\ Put addr of n empty bytes on stack followed by the current string length
	DOES>	
	CELL+ DUP CELL+ SWAP @
;

: $APPEND.CHAR  ( addr char -- )
    SWAP DUP			\ char addr addr
    CELL- @			\ char addr size
    DUP 1+			\ char addr size size+1
    2 PICK CELL- !		\ char addr size -- set string size to +1
    + !				\ Append the char
;

: $CLEAR ( addr count -- , Clear the contents of the string to nothing )
  	DROP
	0 SWAP
	CELL- !
;

: $MAX ( addr count -- count , Puts max size of the string on the stack
       	 For example: 20 $VAR my-string
	     	      my-string $MAX .
	 Pops 20 off the stack )

	DROP CELL- CELL- @
;

: $SIZE ( addr count -- count , Puts current size of the string on the stack
       	  For example: 20 $VAR my-string
	     	       my-string $SIZE .
	  Pops 0 off the stack )

	SWAP DROP
;	

: $SET ( addr-src count-src addr-dest count-dest -- , set the value of a string variable
       	 Example usage: S" hello world" string-variable $SET
	 string-variable will now contain "hello world" )

	\ Bounds check
	2DUP $MAX 3 PICK	\ Stack after: addr-src count-src addr-dest count-dest max-dest count-src
	< IF
		DROP DROP DROP DROP
		CR ." Destination string too small" CR
		QUIT
	THEN

	\ Set the current size
	2 PICK 2 PICK		\ Stack after: addr-src count-src addr-dest count-dest count-src addr-dest
	CELL- !	 		\ Stack after: addr-src count-src addr-dest count-dest 

	\ Set the contents
	DROP SWAP CMOVE
;

: $! $SET ;

: $APPEND ( addr-dest size-dest addr-src size-src -- , append the src string to the dest string
  	    This word uses the pad as a temporary buffer
  	    Example usage:
	    	20 $VAR str-hello
		20 $VAR str-world
		S" hello" str-hello $SET
		S" world" str-world $SET
		str-hello str-world $APPEND
		str-world CR type
		world
		str-hello CR type
		hello world
	  )

	\ Bounds check
	2OVER $MAX		\ Max size of dest string
	3 PICK 2 PICK +		\ current size of src string + current size of dest string
	DUP PAD !      		\ Save the new string size to the PAD
	< IF
		DROP DROP DROP DROP
		CR ." Destination string too small" CR
		QUIT
	THEN

	\ Append source to dest
	2OVER +			\ Get address of end of dest
	SWAP CMOVE		\ Move src into end of dest

	\ Set the new size of dest
	DROP			\ Don't care about the size of dest before appending, drop it
	PAD @			\ Fetch new size from PAD  
	SWAP CELL- !		\ Store the new size
;


: $PREFIX ( addr-src size-src addr-dest size-dest -- , Prefix the src string to the dest string
  	    This word uses the pad as a temporary buffer
  	    Example usage:
	    	20 $VAR str-hello
		20 $VAR str-world
		S" hello" str-hello $SET
		S" world" str-world $SET
		str-hello str-world $PREFIX
		str-world CR type
		hello world
		str-hello CR type
		hello
	  )

	\ Bounds check
	2DUP $MAX		\ Max size of dest string
	3 PICK 2 PICK +		\ current size of src string + current size of dest string
	DUP PAD !      		\ Save the new string size to the PAD
	< IF
		DROP DROP DROP DROP
		CR ." Destination string too small" CR
		QUIT
	THEN
	
	\ Move the dest string to the pad
	2DUP PAD CELL+ SWAP CMOVE

	\ Set the dest string to be the same as the source string
	3 PICK 3 PICK 3 PICK 0 $SET
	DROP OVER

	\ Append the pad to the dest string
	\ Stack looks like this: addr-src size-src addr-dest size-dest
	\ We want it to look like: addr-src addr-dest PAD+CELL addr-dest+size-src size-dest
	-ROT DUP -ROT + ROT PAD CELL+ -ROT
	CMOVE

	\ Set the new size of the dest string
	PAD @
	SWAP CELL- !

	\ Clean up the stack
	DROP
;

: >STRING ( addr n1 n2 -- , only works for numbers >= 0 )
    2 PICK 2 PICK $CLEAR
    SWAP DROP
    
    \ This Loop takes n2 and puts an ascii char on stack for each digit in reverse
    \ ex: 432 -> 50 51 52, where 50, 51, and 52 are ASCII for 2, 3, and 4.
    0 >R
    BEGIN
	10 /MOD 		\ addr rem quo
	SWAP 48 + SWAP		\ addr rem+48 quo
	2 PICK SWAP		\ addr rem+48 addr quo
	R> 1+ >R
	DUP 0=			\ addr rem+48 addr quo 0/-1
    UNTIL
    DROP DROP

    R> 0 DO
	$APPEND.CHAR
    LOOP

    \ Append to the string
    

    \ Put in addr n1 string
;















