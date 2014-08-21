#!/bin/bash

# Batch script to run multiple runs on a limited number of cores using the
# CONDOR batch job manager. The maximum number of processes you would like to be
# running at any given time is MAX_PROCESSES. The total number of runs for this
# job is N_RUNS.
# Mich 2013/07/30

# User parameter space.
WORK_DIR=/home/mich/research/mtc/mtc-sim/mtcg4sim-build
MACRO_DIR=$WORK_DIR/mac/batch
MAX_PROCESSES=3
N_RUNS=100
# End user parameters.

# Actual running script.
for (( i_run=0; i_run<$N_RUNS; i_run++ )); do
	sed 's/SEED_NO/'$i_run'/; s/JOB_SUBMIT_NO/'$i_run'/' \
		$MACRO_DIR/template.mac > $MACRO_DIR/run$i_run.mac
done

#for (( i_run=0; i_run<$N_RUNS; )); do
#	echo "Checking for free queues."
#	N_PROCESSES=$(condor_q | wc -l)
#	(( N_PROCESSES-=6 ))
#	N_FREE_QUEUE=$((MAX_PROCESSES-N_PROCESSES))
#	if [ "$N_FREE_QUEUE" -gt 0 ]; then
#		echo "Found $N_FREE_QUEUE free queues."
#		for (( i_que=0; i_que<$N_FREE_QUEUE; ++i_que )); do
#			condor_run -u vanilla \
#				"$WORK_DIR/mtcg4sim $MACRO_DIR/run$i_run.mac >& \
#				$MACRO_DIR/log$i_run.txt" &
#			echo "Submitted run $i_run."
#			(( i_run++ ))
#			if [ $i_run -ge $N_RUNS ]; then
#				echo "Job finished. Exiting."
#				exit 0
#			fi
#		done
#	fi
#echo "Sleeping for 20 minutes..."
#sleep 20m
#done
