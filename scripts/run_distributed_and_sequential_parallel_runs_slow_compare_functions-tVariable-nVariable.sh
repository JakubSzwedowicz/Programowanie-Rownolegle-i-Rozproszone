#!/usr/bin/env bash

source "$(dirname "$0")/utils.sh"

get_log_dir_and_time_cmd_and_mpi_exec "run_distributed_and_sequential_parallel_runs_slow_compare_functions-tVariable-nVariable.sh" LOG_DIR TIME_CMD NM_EXEC
[ $? -ne 0 ] && exit $?
echo "Using executable: $NM_EXEC"

BASE_COMMANDS=()
BASE_COMMANDS+=("-f 1 -s 1500 -e 3")
BASE_COMMANDS+=("-f 16 -s 300 -e 3")
BASE_COMMANDS+=("-f 17 -s 1000 -e 3")

RUN_CMDS=()
RUNi=0
for CMD in "${BASE_COMMANDS[@]}"; do
    RUN_CMDS+=("mpirun -n 1 $NM_EXEC $CMD -t 1 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))
    RUN_CMDS+=("mpirun -n 1 $NM_EXEC $CMD -t 4 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))
    RUN_CMDS+=("mpirun -n 1 $NM_EXEC $CMD -t 8 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))
    RUN_CMDS+=("mpirun -n 1 $NM_EXEC $CMD -t 16 -l $LOG_DIR/run$RUNi.log")
    ((RUNi++))

    RUN_CMDS+=("mpirun -n 2 $NM_EXEC $CMD -t 1 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))
    RUN_CMDS+=("mpirun -n 2 $NM_EXEC $CMD -t 4 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))
    RUN_CMDS+=("mpirun -n 2 $NM_EXEC $CMD -t 8 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))

    RUN_CMDS+=("mpirun -n 4 $NM_EXEC $CMD -t 1 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))
    RUN_CMDS+=("mpirun -n 4 $NM_EXEC $CMD -t 2 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))
    RUN_CMDS+=("mpirun -n 4 $NM_EXEC $CMD -t 4 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))

    RUN_CMDS+=("mpirun -n 6 $NM_EXEC $CMD -t 1 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))
    RUN_CMDS+=("mpirun -n 6 $NM_EXEC $CMD -t 2 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))
    RUN_CMDS+=("mpirun -n 6 $NM_EXEC $CMD -t 3 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))

    RUN_CMDS+=("mpirun -n 8 $NM_EXEC $CMD -t 1 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))
    RUN_CMDS+=("mpirun -n 8 $NM_EXEC $CMD -t 2 -l  $LOG_DIR/run$RUNi.log")
    ((RUNi++))
done

#let RUNi0=(0)
#let RUNi1=(1)
#let RUNi2=(2)
#for((proc=1; proc<4; proc++)); do
#  RUN_CMDS+=("mpirun -n $proc $NM_EXEC $BASE_COMMAND_F1 -t 1 -l $LOG_DIR/run$RUNi0.log")
#  #RUN_CMDS+=("mpirun -n $proc $NM_EXEC $BASE_COMMAND_F16 -t 1 -l $LOG_DIR/run$RUNi1.log")
#  #RUN_CMDS+=("mpirun -n $proc $NM_EXEC $BASE_COMMAND_F17 -t 1 -l $LOG_DIR/run$RUNi2.log")
#  RUNi0=($RUNi0 + 3)
#  RUNi1=($RUNi1 + 3)
#  RUNi2=($RUNi2 + 3)
#  for((i=1; i<9; i++)); do
#      let T=($i * 2)
#      RUN_CMDS+=("mpirun -n $proc $NM_EXEC $BASE_COMMAND_F1 -t $T -l $LOG_DIR/run$RUNi0.log")
#      #RUN_CMDS+=("mpirun -n $proc $NM_EXEC $BASE_COMMAND_F16 -t $T -l $LOG_DIR/run$RUNi1.log")
#      #RUN_CMDS+=("mpirun -n $proc $NM_EXEC $BASE_COMMAND_F17 -t $T -l $LOG_DIR/run$RUNi2.log")
#      RUNi0=($RUNi0 + 3)
#      RUNi1=($RUNi1 + 3)
#      RUNi2=($RUNi2 + 3)
#  done
#done

run_array_of_cmds_sequentially "$LOG_DIR" RUN_CMDS
merge_logs_and_generate_summary "$LOG_DIR" "function" RUN_CMDS
