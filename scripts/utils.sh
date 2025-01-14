#!/usr/bin/env bash

find_executable() {
    local exec_name=$1
    local exec_path=$(find . -name "$exec_name" -type f -executable 2>/dev/null | head -n1)
    if [[ -z "$exec_path" ]]; then
        echo -e "ERROR: Could not find an executable named '$exec_name' in the CURRENT DIRECTORY or SUBDIRECTORIES. Are you calling inside /scripts/?" >&2
        exit 1
    fi
    echo "$exec_path"
    return 0
}

function setup_log_directory() {
  # Create a timestamped directory for logs so that each run doesn't overwrite the previous logs
  local LOG_DIR="logs/$1/logs_$(date +%Y%m%d_%H%M%S)"
  mkdir -p "$LOG_DIR"
  echo "$LOG_DIR"
  return 0
}

function get_log_dir_and_time_cmd_and_nm_exec() {
  local LOG_SUBDIRECTORY="$1"
  local -n _LOG_DIR="$2"
  local -n _TIME_CMD="$3"
  local -n _NM_EXEC="$4"

  _LOG_DIR=$(setup_log_directory "$LOG_SUBDIRECTORY")
  _TIME_CMD="/usr/bin/time"
  _NM_EXEC=$(find_executable "app")
  return $?  # Return the exit code of find_executable
}

function get_log_dir_and_time_cmd_and_mpi_exec() {
  local LOG_SUBDIRECTORY="$1"
  local -n _LOG_DIR="$2"
  local -n _TIME_CMD="$3"
  local -n _NM_EXEC="$4"

  _LOG_DIR=$(setup_log_directory "$LOG_SUBDIRECTORY")
  _TIME_CMD="/usr/bin/time"
  _NM_EXEC=$(find_executable "appMPI")
  return $?  # Return the exit code of find_executable
}

function run_array_of_cmds_sequentially() {
  local LOG_DIR="$1"
  local array_name="$2"
  if [[ -z "$array_name" ]]; then
    echo "Error: You must pass the name of an array." >&2
    return 1
  fi
  local -n _RUN_CMDS="$array_name"

  echo "This script will run ${#_RUN_CMDS[@]} processes SEQUENTIALLY. Commands are:"
  for ((i=0; i<${#_RUN_CMDS[@]}; i++)); do
    echo "Command $i: ${_RUN_CMDS[$i]}"
  done

  continue_or_abort

  for ((i=0; i<${#_RUN_CMDS[@]}; i++)); do
    echo "Starting command $i..."
    ${_RUN_CMDS[$i]} > "$LOG_DIR/run$((i)).out" 2> "$LOG_DIR/run$((i)).time"
  done
}

function run_array_of_cmds_in_parallel() {
  local LOG_DIR="$1"
  local array_name="$2"
  if [[ -z "$array_name" ]]; then
    echo "Error: You must pass the name of an array to _RUN_CMDS_in_parallel."
    return 1
  fi
  local -n _RUN_CMDS="$array_name"

  echo "This script will run ${#_RUN_CMDS[@]} processes in PARALLEL and might drain resources. Commands are:"
  for ((i=0; i<${#_RUN_CMDS[@]}; i++)); do
    echo "Command $i: ${_RUN_CMDS[$i]}"
  done

  continue_or_abort

  local PIDS=()
  for ((i=0; i<${#_RUN_CMDS[@]}; i++)); do
    echo "Starting command $i..."
    {
      ${_RUN_CMDS[$i]} > "$LOG_DIR/run$((i)).out" 2> "$LOG_DIR/run$((i)).time"
    } &
    PIDS+=($!)
  done
  # Wait for all background jobs to complete
  wait "${PIDS[@]}"
}

function merge_logs_and_generate_summary() {
  local LOG_DIR="$1"
  local PLOT_BY="$2"
  local array_name="$3"
  if [[ -z "$array_name" ]]; then
    echo "Error: You must pass the name of an array."
    return 1
  fi
  local -n _RUN_CMDS="$array_name"

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
  python3 "$(dirname "$0")/process_logs.py" --log_dir "$LOG_DIR" --graph --table --plot_by "$PLOT_BY"

  echo "Done!"
  return 0;
}


function continue_or_abort() {
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
}