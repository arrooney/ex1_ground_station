if [ -z $1 ]
then
    cat output_log.txt
else
    tail -n $1 output_log.txt
fi
