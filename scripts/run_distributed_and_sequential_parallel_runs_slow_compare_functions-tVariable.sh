#!/usr/bin/env bash

source "$(dirname "$0")/utils.sh"

get_log_dir_and_time_cmd_and_mpi_exec "run_distributed_and_sequential_parallel_runs_slow_compare_functions-tVariable.sh" LOG_DIR TIME_CMD NM_EXEC
[ $? -ne 0 ] && exit $?
echo "Using executable: $NM_EXEC"

BASE_COMMAND_F1="-f 1 -s 1500 -e 3"
BASE_COMMAND_F16="-f 16 -s 300 -e 3"
BASE_COMMAND_F17="-f 17 -s 1000 -e 3"

RUN_CMDS=()
RUN_CMDS+=("mpirun -n 4 $NM_EXEC $BASE_COMMAND_F1 -t 1 -l $LOG_DIR/run0.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC $BASE_COMMAND_F16 -t 1 -l $LOG_DIR/run1.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC $BASE_COMMAND_F17 -t 1 -l $LOG_DIR/run2.log")
for((i=1; i<9; i++)); do
    let RUNi0=($i * 3)
    let RUNi1=($i * 3 + 1)
    let RUNi2=($i * 3 + 2)
    let T=($i * 2)
    RUN_CMDS+=("mpirun -n 4 $NM_EXEC $BASE_COMMAND_F1 -t $T -l $LOG_DIR/run$RUNi0.log")
    RUN_CMDS+=("mpirun -n 4 $NM_EXEC $BASE_COMMAND_F16 -t $T -l $LOG_DIR/run$RUNi1.log")
    RUN_CMDS+=("mpirun -n 4 $NM_EXEC $BASE_COMMAND_F17 -t $T -l $LOG_DIR/run$RUNi2.log")
done

run_array_of_cmds_sequentially "$LOG_DIR" RUN_CMDS
merge_logs_and_generate_summary "$LOG_DIR" "threads" RUN_CMDS
