DEBUG.UP			\ Uploads a file, make sure it uploads successfully
." Make sure DEBUG.UP ran"
WAIT

S" eps|110" GOM.COMMAND		\ Turns power on to Athena, if it fails, it prints this message:
				\ EPS conf: output set error: x
				\ Make sure it doesn't print this message.
				\ Same as eps output 1 1 0

WAIT

S" athena|start" GOM.COMMAND	\ Prints, on success:
				\ athena: diag: data collection and commanding enabled
				\ Make sure it prints this
WAIT

S" athena|51" GOM.COMMAND	\ Prints, on success:
				\ ATHENA: crc: 0xX
				\ Make sure this gets printed
WAIT

DEBUG.DOWN			\ Deletes a file, make sure it gets deleted
WAIT

S" athena|stop" GOM.COMMAND	\ Due to debug.down, this will not print a message

S" eps|100" GOM.COMMAND		\ Turns power OFF to Athena, if it fails, it prints this message:
				\ EPS conf: output set error: x
				\ Make sure it doesn't print this message.
				\ Same as eps output 1 0 0
WAIT