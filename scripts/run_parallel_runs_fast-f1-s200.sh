#!/usr/bin/env bash

source "$(dirname "$0")/utils.sh"

get_log_dir_and_time_cmd_and_nm_exec LOG_DIR TIME_CMD NM_EXEC
echo "Using executable: $NM_EXEC"

BASE_COMMAND="-f 1 -s 200 -e 3"

RUN_CMDS=()
RUN_CMDS+=("$TIME_CMD $NM_EXEC $BASE_COMMAND -t 1 -l $LOG_DIR/run0.log")
for((i=1; i<9; i++)); do
    let T=($i * 2)
    RUN_CMDS+=("$TIME_CMD $NM_EXEC $BASE_COMMAND -t $T -l $LOG_DIR/run$i.log")
done

run_array_of_cmds_sequentially $LOG_DIR RUN_CMDS
merge_logs_and_generate_summary "$LOG_DIR" "threads" RUN_CMDS