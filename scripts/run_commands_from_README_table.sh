#!/usr/bin/env bash

read -r -p "This script will run 6 processes in parallel
and will drain resources heavily for few (3min on 9800x3d) minutes (all 6 will use 100% of logical threads. Could be 50-60% of total CPU, depends...)
Continue? [y/N] " answer
case "${answer,,}" in  # convert to lowercase
    y|yes)
        echo "Ok, starting the runs..."
        ;;
    *)
        echo "Aborting."
        exit 1
        ;;
esac

source "$(dirname "$0")/utils.sh"

NM_EXEC=$(find_executable "app")
echo "Using executable: $NM_EXEC"

echo "Spawning multiple processes in parallel (6 total)..."

# Create a timestamped directory for logs so that each run doesn't overwrite the previous logs
LOG_DIR="logs/logs_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$LOG_DIR"

TIME_CMD="/usr/bin/time"
RUN_CMDS=()

# Faster runs for testing
#RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s 200 -e 6 -l $LOG_DIR/run0.log")
#RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s 300 -e 9 -l $LOG_DIR/run1.log")

# Default runs
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s 1000 -e 6 -l $LOG_DIR/run0.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 1 -s 1000 -e 9 -l $LOG_DIR/run1.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 16 -s 200 -e 6 -l $LOG_DIR/run2.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 16 -s 250 -e 6 -l $LOG_DIR/run3.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 17 -s 500 -e 6 -l $LOG_DIR/run4.log")
RUN_CMDS+=("$TIME_CMD $NM_EXEC -f 17 -s 500 -e 9 -l $LOG_DIR/run5.log")


PIDS=()
for ((i=0; i<${#RUN_CMDS[@]}; i++)); do
  {
    ${RUN_CMDS[$i]} > "$LOG_DIR/run$((i)).out" 2> "$LOG_DIR/run$((i)).time"
  } &
  PIDS+=($!)
done
# Wait for all background jobs to complete
wait "${PIDS[@]}"


for ((i=0; i<${#RUN_CMDS[@]}; i++)); do
  if [[ -f "${LOG_DIR}/run$i.log" ]] \
    && [[ -f "${LOG_DIR}/run$i.out" ]] \
    && [[ -f "${LOG_DIR}/run$i.time" ]]; then
    # Merge them into one .log file
    echo "=== CLI ==="                          >  "${LOG_DIR}/run${i}_merged.log"
    echo "Command: ${RUN_CMDS[$i]}"             >> "${LOG_DIR}/run${i}_merged.log"
    echo "=== Logger ==="                       >> "${LOG_DIR}/run${i}_merged.log"
    cat  "${LOG_DIR}/run$i.log"                 >> "${LOG_DIR}/run${i}_merged.log"
    echo "=== STDOUT ==="                       >> "${LOG_DIR}/run${i}_merged.log"
    cat  "${LOG_DIR}/run$i.out"                 >> "${LOG_DIR}/run${i}_merged.log"
    echo "=== TIME/STDERR ==="                  >> "${LOG_DIR}/run${i}_merged.log"
    cat  "${LOG_DIR}/run$i.time"                >> "${LOG_DIR}/run${i}_merged.log"

    rm -f "${LOG_DIR}/run$i.log" "${LOG_DIR}/run$i.out" "${LOG_DIR}/run$i.time"
  fi
done

echo "Logs have been moved to: $LOG_DIR"

echo "Now generating a summary of the runs..."
python3 "$(dirname "$0")/generate_README_table.py" "$LOG_DIR"

echo "Done!"