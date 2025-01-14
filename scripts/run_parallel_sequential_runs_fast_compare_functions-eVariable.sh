#!/usr/bin/env bash

source "$(dirname "$0")/utils.sh"

get_log_dir_and_time_cmd_and_nm_exec "run_parallel_sequential_runs_fast_compare_functions-eVariable" LOG_DIR TIME_CMD NM_EXEC
[ $? -ne 0 ] && exit $?
echo "Using executable: $NM_EXEC"

RUN_CMDS=()
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s 500 -e 1 -l $LOG_DIR/run0.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s 500 -e 3 -l $LOG_DIR/run1.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s 500 -e 6 -l $LOG_DIR/run2.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s 500 -e 9 -l $LOG_DIR/run3.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s 500 -e 12 -l $LOG_DIR/run4.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 16 -s 150 -e 1 -l $LOG_DIR/run5.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 16 -s 150 -e 3 -l $LOG_DIR/run6.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 16 -s 150 -e 6 -l $LOG_DIR/run7.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 16 -s 150 -e 9 -l $LOG_DIR/run8.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 16 -s 150 -e 12 -l $LOG_DIR/run9.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 17 -s 200 -e 1 -l $LOG_DIR/run10.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 17 -s 200 -e 3 -l $LOG_DIR/run11.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 17 -s 200 -e 6 -l $LOG_DIR/run12.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 17 -s 200 -e 9 -l $LOG_DIR/run13.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 17 -s 200 -e 12 -l $LOG_DIR/run14.log")

run_array_of_cmds_in_parallel "$LOG_DIR" RUN_CMDS
merge_logs_and_generate_summary "$LOG_DIR" "epsilon" RUN_CMDS
