#!/usr/bin/env bash

source "$(dirname "$0")/utils.sh"

get_log_dir_and_time_cmd_and_mpi_exec "run_distributed_and_sequential_parallel_runs_fast_compare_functions-eVariable" LOG_DIR TIME_CMD NM_EXEC
[ $? -ne 0 ] && exit $?
echo "Using executable: $NM_EXEC"

RUN_CMDS=()
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 1 -s 500 -e 1 -t 4 -l $LOG_DIR/run0.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 1 -s 500 -e 3 -t 4 -l $LOG_DIR/run1.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 1 -s 500 -e 6 -t 4 -l $LOG_DIR/run2.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 1 -s 500 -e 9 -t 4 -l $LOG_DIR/run3.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 1 -s 500 -e 12 -t 4 -l $LOG_DIR/run4.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 16 -s 150 -e 1 -t 4 -l $LOG_DIR/run5.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 16 -s 150 -e 3 -t 4 -l $LOG_DIR/run6.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 16 -s 150 -e 6 -t 4 -l $LOG_DIR/run7.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 16 -s 150 -e 9 -t 4 -l $LOG_DIR/run8.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 16 -s 150 -e 12 -t 4 -l $LOG_DIR/run9.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 17 -s 200 -e 1 -t 4 -l $LOG_DIR/run10.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 17 -s 200 -e 3 -t 4 -l $LOG_DIR/run11.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 17 -s 200 -e 6 -t 4 -l $LOG_DIR/run12.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 17 -s 200 -e 9 -t 4 -l $LOG_DIR/run13.log")
RUN_CMDS+=("mpirun -n 4 $NM_EXEC -f 17 -s 200 -e 12 t 4 -l $LOG_DIR/run14.log")

run_array_of_cmds_sequentially "$LOG_DIR" RUN_CMDS
merge_logs_and_generate_summary "$LOG_DIR" "epsilon" RUN_CMDS
