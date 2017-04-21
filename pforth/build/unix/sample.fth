\ Sample Forth script

anew task-sample.fth

: square ( n -- n*n, square of number )
  DUP *
;

: test.square ( --, test of square )
  CR ." 3 squared = "
  3 square . CR
;