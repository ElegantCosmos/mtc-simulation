#!/bin/sh

###### Instructions #########
# This shell script runs multiple versions of this program each corresponding to
# a specific neutrino energy that can be set by the user below in the user
# variables section.
#############################

###### User variables #######
#TRUNKDIR=/home/mich/data/school/research/mtc/mtcg4sim/material/MTCG4PrimaryParticleGen
#WORKDIR=$TRUNKDIR/trunk
WORKDIR=$(pwd)
i=1800 # Neutrino energy lower bound in keV
i_MAX=10000 # Neutrino energy higher bound in keV
#############################

###### Begin code ###########
cd $WORKDIR
i=1800
IN_CPP_FILE="generatePrimaryParticleData.cpp"
IN_MAKEFILE="Makefile_template"
echo "IN_CPP_FILE=$IN_CPP_FILE"
while [ $i -le ${i_MAX} ]; do
	echo "i=$i"
	NEUTRINO_KE=$(echo "scale=3; $i/1000" | bc) # Use bc for float calculation.
	
	j="$(printf "%05d" $i)"
	# Generate specific source code for this energy.
	OUT_CPP_FILE="generatePrimaryParticleData_${j}.cpp"
	echo "OUT_CPP_FILE=$OUT_CPP_FILE"
	sed 's/^\(#define\s\+PRIMARY_NEUTRINO_KINETIC_ENERGY\s\+\)[0-9]\.*[0-9]*/\1'${NEUTRINO_KE}'/g; s/primaryParticleData\.dat/'primaryParticleData_${j}.dat'/g' ${WORKDIR}/${IN_CPP_FILE} > ${WORKDIR}/${OUT_CPP_FILE}

	# Generate specific makefile for this energy.
	OUT_MAKEFILE="Makefile"
	sed 's/VARIABLE_NEUTRINO_ENERGY/'${j}'/g' ${WORKDIR}/${IN_MAKEFILE} > $WORKDIR/$OUT_MAKEFILE
	
	# Make and run source.
	make
	./generatePrimaryParticleData_${j}

	i=$(expr $i + 1)
done

exit 0


