#!/bin/sh
#NOW=`date +'%y%m%d%k%M%S'`
#mv /mnt/datadisk/school/research/MTC/MTCG4Sim/output/now /mnt/datadisk/school/research/MTC/MTCG4Sim/output/${NOW}
rm -rf /mnt/datadisk/school/research/MTC/MTCG4Sim/output/now
mkdir /mnt/datadisk/school/research/MTC/MTCG4Sim/output/now
make clean
make
./mtc run.mac | tee ../output/now/log.txt
cp run.mac /mnt/datadisk/school/research/MTC/MTCG4Sim/output/now/
