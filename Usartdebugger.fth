: DEBUG.ENABLE ( -- , enable debug printing for 10 minutes when transferring adcs file from nanomind. )

    S" debug|1" GOM.COMMAND

;

: DEBUG.DISABLE ( -- , disable Usart debug printing for 10 minutes when transferring adcs file from nanomind.)
    S" debug|0" GOM.COMMAND

;
