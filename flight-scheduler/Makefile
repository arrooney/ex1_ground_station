SCHED_EXEC := create-flight-schedule
SCHED_SOURCE := flight_scheduler.c

ADCS_EXEC := create-adcs-command
ADCS_SOURCE := command_gen.c

GCC_OPT := -Wall -Wextra

all: adcs schedule

adcs:
	gcc $(GCC_OPT) $(ADCS_SOURCE) -o $(ADCS_EXEC)

schedule:
	gcc $(GCC_OPT) $(SCHED_SOURCE) -o $(SCHED_EXEC)

clean:
	rm -f $(SCHED_EXEC) $(ADCS_EXEC) *~
