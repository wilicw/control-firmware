#!/usr/bin/env bash
FULL_PATH_TO_SCRIPT="$(realpath -s "$0")"
SCRIPT_DIRECTORY="$(dirname "$FULL_PATH_TO_SCRIPT")"
RETURN_CODE_SUCCESS=0
RETURN_CODE_ERROR=1
FILE_LIST="$(find "$SCRIPT_DIRECTORY/Core" "$SCRIPT_DIRECTORY/Drivers" "$SCRIPT_DIRECTORY/FileX" "$SCRIPT_DIRECTORY/AZURE_RTOS" | grep -E ".*\.(ino|cpp|cc|c|h|hpp|hh)$")"
IFS=$'\n' read -r -d '' -a FILE_LIST_ARRAY <<< "$FILE_LIST"
num_files="${#FILE_LIST_ARRAY[@]}"
for i in "${!FILE_LIST_ARRAY[@]}"; do
    file="${FILE_LIST_ARRAY["$i"]}"
done
clang-format -i --style=file "${FILE_LIST_ARRAY[@]}"

