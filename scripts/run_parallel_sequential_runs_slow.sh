#!/usr/bin/env bash

source "$(dirname "$0")/utils.sh"

get_log_dir_and_time_cmd_and_nm_exec LOG_DIR TIME_CMD NM_EXEC
[ $? -ne 0 ] && exit $?
echo "Using executable: $NM_EXEC"

RUN_CMDS=()
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s 1000 -e 6 -l $LOG_DIR/run0.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s 1000 -e 9 -l $LOG_DIR/run1.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 16 -s 200 -e 6 -l $LOG_DIR/run2.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 16 -s 250 -e 6 -l $LOG_DIR/run3.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 17 -s 500 -e 6 -l $LOG_DIR/run4.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 17 -s 500 -e 9 -l $LOG_DIR/run5.log")

run_array_of_cmds_in_parallel "$LOG_DIR" RUN_CMDS
merge_logs_and_generate_summary "$LOG_DIR" "function" RUN_CMDS
