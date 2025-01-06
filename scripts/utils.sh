#!/usr/bin/env bash

find_executable() {
    local exec_name=$1
    local exec_path=$(find . -name "$exec_name" -type f -executable 2>/dev/null | head -n1)
    if [[ -z "$exec_path" ]]; then
        echo "ERROR: Could not find an executable named '$exec_name' in the CURRENT DIRECTORY or SUBDIRECTORIES.
        Are you calling inside /scripts/?"
        exit 1
    fi
    echo "$exec_path"
    return 0
}

function setup_log_directory() {
  # Create a timestamped directory for logs so that each run doesn't overwrite the previous logs
  local LOG_DIR="logs/logs_$(date +%Y%m%d_%H%M%S)"
  mkdir -p "$LOG_DIR"
  echo "$LOG_DIR"
  return 0
}
function get_log_dir_and_time_cmd_and_nm_exec() {
  local -n _LOG_DIR="$1"
  local -n _TIME_CMD="$2"
  local -n _NM_EXEC="$3"

  _LOG_DIR=$(setup_log_directory)
  _TIME_CMD="/usr/bin/time"
  _NM_EXEC=$(find_executable "app")

}

function run_array_of_cmds_in_parallel() {
  local LOG_DIR="$1"
  local array_name="$2"
  if [[ -z "$array_name" ]]; then
    echo "Error: You must pass the name of an array to _RUN_CMDS_in_parallel."
    return 1
  fi
  local -n _RUN_CMDS="$array_name"

  echo "This script will run ${#_RUN_CMDS[@]} processes in parallel and might drain resources. Commands are:"
  for ((i=0; i<${#_RUN_CMDS[@]}; i++)); do
    echo "Command $i: ${_RUN_CMDS[$i]}"
  done

  read -r -p "Continue? [y/N] " answer
  case "${answer,,}" in  # convert to lowercase
      y|yes)
          echo "Ok, starting the runs..."
          ;;
      *)
          echo "Aborting."
          exit 1
          ;;
  esac

  local PIDS=()
  for ((i=0; i<${#_RUN_CMDS[@]}; i++)); do
    {
      ${_RUN_CMDS[$i]} > "$LOG_DIR/run$((i)).out" 2> "$LOG_DIR/run$((i)).time"
    } &
    PIDS+=($!)
  done
  # Wait for all background jobs to complete
  wait "${PIDS[@]}"


  for ((i=0; i<${#_RUN_CMDS[@]}; i++)); do
    if [[ -f "${LOG_DIR}/run$i.log" ]] \
      && [[ -f "${LOG_DIR}/run$i.out" ]] \
      && [[ -f "${LOG_DIR}/run$i.time" ]]; then
      # Merge them into one .log file
      echo "=== CLI ==="                          >  "${LOG_DIR}/run${i}_merged.log"
      echo "Command: ${_RUN_CMDS[$i]}"             >> "${LOG_DIR}/run${i}_merged.log"
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
  return 0;
}