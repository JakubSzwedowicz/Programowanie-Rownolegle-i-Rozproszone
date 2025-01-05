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
