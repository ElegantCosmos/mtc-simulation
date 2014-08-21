Executable = mtcg4sim
Universe   = vanilla
getenv     = true
input      = mac/batch/run$(Process).mac

output     = OUTPUT_FILE
error      = ERROR_FILE
log        = LOG_FILE

### Queue ###
Queue N_JOBS
