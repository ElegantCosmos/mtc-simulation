Executable = mtcg4sim
Universe   = vanilla
getenv     = true
input      = mac/batch/run$(Process).mac
output     = /data/mtc-sim/cosmicray_mu/run$(Process).out
error      = /data/mtc-sim/cosmicray_mu/run$(Process).err
log        = /data/mtc-sim/cosmicray_mu/run$(Process).log
#output     = /data/mtc-sim/2gev_mu/run$(Process).out
#error      = /data/mtc-sim/2gev_mu/run$(Process).err
#log        = /data/mtc-sim/2gev_mu/run$(Process).log
#output     = /data/mtc-sim/283gev_mu/run$(Process).out
#error      = /data/mtc-sim/283gev_mu/run$(Process).err
#log        = /data/mtc-sim/283gev_mu/run$(Process).log
Queue 100
#Queue 10
#Queue 1
