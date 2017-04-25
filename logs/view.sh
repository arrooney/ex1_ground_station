# ./view.sh - view entire output_log in terminal
# ./view.sh x - view last x lines of output_log in terminal
#	./view.sh 20 - shows the last 20 lines of the output_log

if [ -z $1 ]
then
    cat output_log.txt
else
    tail -n $1 output_log.txt
fi
