#!/bin/bash

HISTFILE=~/.myhistory
HISTSIZE=20

declare -a COMMANDS=()

myhistory() {
  if [[ $1 == "-c" ]]; then
    COMMANDS=()
    echo "Command history cleared."
  elif [[ $1 == "-e" ]]; then
    index=$(( $2 - 1 ))
    if [[ $index -lt ${#COMMANDS[@]} && $index -ge 0 ]]; then
      cmd=${COMMANDS[$index]}
      echo "> $cmd"
      eval "$cmd"
    else
      echo "Invalid command index."
    fi
  else
    for i in "${!COMMANDS[@]}"; do
      echo "$(( $i + 1 )). ${COMMANDS[$i]}"
    done
  fi
}

add_to_history() {
  local command="$(history 1 | sed 's/^[ ]*[0-9]*[ ]*//')"
  if [[ ! -z "$command" && "$command" != "myhistory"* ]]; then
    COMMANDS=("${COMMANDS[@]}" "$command")
    if [[ ${#COMMANDS[@]} -gt $HISTSIZE ]]; then
      COMMANDS=("${COMMANDS[@]:1}")
    fi
  fi
}

trap 'add_to_history' DEBUG

while true; do
  read -e -p "> " command
  if [[ $command == "myhistory"* ]]; then
    args=($command)
    myhistory ${args[@]:1}
  else
    eval "$command"
  fi
done
