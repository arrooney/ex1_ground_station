include gom2.fth

wait
s" rparam download 15 0" GOM
wait
s" rparam copy 0 1" GOM
wait
s" rparam send" GOM
wait
s" rparam download 15 1" GOM
wait
s" rparam getall" GOM
wait
s" rparam set reboot_in 300" GOM
wait
s" rparam set tx_baud 9600" GOM
wait
s" rparam set rx_baud 9600" GOM
wait
s" rparam getall" GOM
wait
s" rparam copy 1 0" GOM
wait
s" rparam send" GOM
wait
s" eps hk" GOM
wait
s" rparam save 0 0" GOM
wait
s" rparam send" GOM
