#!/bin/sh

# This script can be used to generate multiple macros to run multiple runs each
# in parallel. This can cut down on simulation time. Geant4 uses one CPU for one
# run by default. So if you have 10 CPUs, you can run 10 separate runs
# simultaneously. Just put in what you would like in the templace.mac macro
# provided and change the user variables below and run this script.
# -- Mich 2013/02/01 --

###### User variables #######
TRUNK_DIR=/home/mich/research/mtc/mtc-sim/mtcg4sim/trunk
WORK_DIR=$TRUNK_DIR/../trunk-build
MAC_DIR=$WORK_DIR/mac/batch
N_RUNS=100
#############################
cd $WORK_DIR
i=0
echo `expr $i + 1`
IN_FILE_NAME="template.mac"
echo "IN_FILE_NAME=$IN_FILE_NAME"
while [ $i -lt ${N_RUNS} ]; do
	echo "i=$i"
	#RND_NO=`od -An -N3 -l /dev/random | sed 's/^ *\(.*\) *$/\1/'`
	RND_NO=0
	echo "Random number = $RND_NO"
	OUT_FILE_NAME="run${i}.mac"
	echo "OUT_FILE_NAME=$OUT_FILE_NAME"
	sed 's/SEED_NO/'${RND_NO}'/g; s/JOB_SUBMIT_NO/'${i}'/g' \
    ${MAC_DIR}/${IN_FILE_NAME} > ${MAC_DIR}/${OUT_FILE_NAME}
	#./mtcg4sim ${MAC_DIR}/template_${i}.mac > log_${i}.txt &
	i=`expr $i + 1`
done

exit 0

