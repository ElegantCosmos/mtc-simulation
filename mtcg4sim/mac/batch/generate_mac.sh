#!/bin/sh

# This script can be used to generate multiple macros to run multiple runs each
# in parallel. This can cut down on simulation time. Geant4 uses one CPU for one
# run by default. So if you have 10 CPUs, you can run 10 separate runs
# simultaneously. Just put in what you would like in the templace.mac macro
# provided and change the user variables below and run this script.
# -- Mich 2013/02/01 --

###################
### User variables
###################

### Directories.
TRUNK_DIR=/home/mich/research/mtc/mtc-sim/mtcg4sim
WORK_DIR=$TRUNK_DIR/../mtcg4sim-build
MAC_DIR=$WORK_DIR/mac/batch
CONDOR_OUTPUT_BASE=/data/mich/mtc-sim
#CONDOR_OUTPUT_DIR=${CONDOR_OUTPUT_BASE}/cosmogenic_isotopes_of_sea_level_spectrum_mu_1kevt_1mmCut_20140211
CONDOR_OUTPUT_DIR=${CONDOR_OUTPUT_BASE}/neutron/isotropic_from_center/2.5mev

### Number of runs and Condor execution queue.
N_EVENTS=100 # Number of events per run. One run per job.
N_JOBS=100 # Number of jobs in queue.

### File names.
IN_FILE_1=${MAC_DIR}/template.mac
IN_FILE_2=${MAC_DIR}/condor_batch_template.cmd
CONDOR_OUTPUT_FILE=${CONDOR_OUTPUT_DIR}/'run\$\(Process\).out'
CONDOR_ERROR_FILE=${CONDOR_OUTPUT_DIR}/'run\$\(Process\).err'
CONDOR_LOG_FILE=${CONDOR_OUTPUT_DIR}/'run\$\(Process\).log'
###### End of user variables #######


#########################
### Begin execution code
#########################

rm -rf ${CONDOR_OUTPUT_DIR}/*
cd $WORK_DIR
### Create Condor .cmd file.
OUT_FILE_2=${MAC_DIR}/condor_batch.cmd
#echo "CONDOR_OUTPUT_FILE=${CONDOR_OUTPUT_FILE}"
sed -e s:OUTPUT_FILE:${CONDOR_OUTPUT_FILE}:g \
	-e s:ERROR_FILE:${CONDOR_ERROR_FILE}:g \
	-e s:LOG_FILE:${CONDOR_LOG_FILE}:g \
	-e s:N_JOBS:${N_JOBS}:g \
	${IN_FILE_2} > ${OUT_FILE_2}

### Create macro file, one for each run.
iQue=0
echo $(expr $iQue + 1)
echo "IN_FILE_1=$IN_FILE_1"
while [ $iQue -lt ${N_JOBS} ]; do
	echo "Loop number = $iQue"
	#RND_NO=`od -An -N3 -l /dev/random | sed 's/^ *\(.*\) *$/\1/'`
	RND_NO=${iQue}
	echo "Random number = $RND_NO"
	OUT_FILE_1=${MAC_DIR}/"run${iQue}.mac"
	echo "OUT_FILE_1=$OUT_FILE_1"
	sed -e s:N_EVENTS:${N_EVENTS}:g \
		-e s:JOB_SUBMIT_NO:${iQue}:g \
		-e s:CONDOR_OUTPUT_DIR:${CONDOR_OUTPUT_DIR}\/:g \
		${IN_FILE_1} > ${OUT_FILE_1}
	#./mtcg4sim ${MAC_DIR}/template_${iQue}.mac > log_${iQue}.txt &
	iQue=$(expr $iQue + 1)
done

exit 0

