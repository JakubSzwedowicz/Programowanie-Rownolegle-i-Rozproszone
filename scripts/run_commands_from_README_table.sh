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

# We will run 6 processes in parallel, each measured by 'time'.
# We redirect:
#   - Standard output to "runX.out"
#   - Time (and any errors) to "runX.time"
# so each run’s logs do not intermix.

echo "Spawning multiple processes in parallel (6 total)..."

# Create a timestamped directory for logs so that each run doesn't overwrite the previous logs
LOG_DIR="logs/logs_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$LOG_DIR"

{
  time "$NM_EXEC" -f 1 -s 1000 -e 6 \
  > "${LOG_DIR}/run1.out" 2> "${LOG_DIR}/run1.time"
} &
PID1=$!

{
  time "$NM_EXEC" -f 1 -s 1000 -e 9 \
  > "${LOG_DIR}/run2.out" 2> "${LOG_DIR}/run2.time"
} &
PID2=$!

{
  time "$NM_EXEC" -f 16 -s 200 -e 6 \
  > "${LOG_DIR}/run3.out" 2> "${LOG_DIR}/run3.time"
} &
PID3=$!

{
  time "$NM_EXEC" -f 16 -s 250 -e 6 \
  > "${LOG_DIR}/run4.out" 2> "${LOG_DIR}/run4.time"
} &
PID4=$!

{
  time "$NM_EXEC" -f 17 -s 500 -e 6 \
  > "${LOG_DIR}/run5.out" 2> "${LOG_DIR}/run5.time"
} &
PID5=$!

{
  time "$NM_EXEC" -f 17 -s 500 -e 9 \
  > "${LOG_DIR}/run6.out" 2> "${LOG_DIR}/run6.time"
} &
PID6=$!

# Wait for all background jobs to complete
wait "$PID1" "$PID2" "$PID3" "$PID4" "$PID5" "$PID6"

for i in {1..6}; do
  if [[ -f "${LOG_DIR}/run$i.out" ]] && [[ -f "${LOG_DIR}/run$i.time" ]]; then
    # Merge them into one .log file
    echo "=== STDOUT ==="              >  "${LOG_DIR}/run${i}_merged.log"
    cat  "${LOG_DIR}/run$i.out"                 >> "${LOG_DIR}/run${i}_merged.log"
    echo -e "\n\n=== TIME/STDERR ===" >> "${LOG_DIR}/run${i}_merged.log"
    cat  "${LOG_DIR}/run$i.time"                >> "${LOG_DIR}/run${i}_merged.log"

    rm -f "${LOG_DIR}/run$i.out" "${LOG_DIR}/run$i.time"
  fi
done

echo "Logs have been moved to: $LOG_DIR"
echo "Done!"