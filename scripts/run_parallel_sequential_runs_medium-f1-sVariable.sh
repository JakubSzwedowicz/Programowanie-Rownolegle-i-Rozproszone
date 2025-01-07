#!/usr/bin/env bash

source "$(dirname "$0")/utils.sh"

get_log_dir_and_time_cmd_and_nm_exec "run_parallel_sequential_runs_medium-f1-sVariable" LOG_DIR TIME_CMD NM_EXEC
[ $? -ne 0 ] && exit $?
echo "Using executable: $NM_EXEC"

RUN_CMDS=()
for((i=0; i<10; i++)); do
    let T=($i + 1)
    RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s ${T}00 -e 3 -l $LOG_DIR/run$i.log")
done

run_array_of_cmds_in_parallel $LOG_DIR RUN_CMDS
merge_logs_and_generate_summary "$LOG_DIR" "size" RUN_CMDS