#!/bin/zsh

FIFO_PATH="/tmp/physics_fifo"

if [ ! -p "$FIFO_PATH" ]; then
  echo "FIFO does not exist: $FIFO_PATH"
  exit 1
fi

while true; do
  echo "Enter command in the format:"
  echo "  add <shapeType (C/S)> <Name> <Size> <X_position> <Y_position> <X_velocity> <Y_velocity>"
  echo "  remove <Name>"
  echo "  set gravity <value>"
  echo "  set coe <value>"
  echo "  random <count>"
  echo "  clear"
  echo "  exit"
  printf "> "
  read -r INPUT

  if [[ "$INPUT" == "exit" ]]; then
    echo "Exiting..."
    break
  fi

  COMMAND=$(echo "$INPUT" | awk '{print $1}')

  case "$COMMAND" in
    add)
      ARG_COUNT=$(echo "$INPUT" | wc -w)
      if [ "$ARG_COUNT" -ne 8 ]; then
        echo "Invalid 'add' command format. Expected 7 arguments after 'add'."
        continue
      fi
      ;;
    remove)
      ARG_COUNT=$(echo "$INPUT" | wc -w)
      if [ "$ARG_COUNT" -ne 2 ]; then
        echo "Invalid 'remove' command format. Expected 1 argument after 'remove'."
        continue
      fi
      ;;
    set)
      ARG_COUNT=$(echo "$INPUT" | wc -w)
      if [ "$ARG_COUNT" -ne 3 ]; then
        echo "Invalid 'set' command format. Expected 2 arguments after 'set'."
        continue
      fi
      ;;
    random)
      ARG_COUNT=$(echo "$INPUT" | wc -w)
      if [ "$ARG_COUNT" -ne 2 ]; then
        echo "Invalid 'random' command format. Expected 1 argument after 'random'."
        continue
      fi
      ;;
    clear)
      ARG_COUNT=$(echo "$INPUT" | wc -w)
      if [ "$ARG_COUNT" -ne 1 ]; then
        echo "Invalid 'clear' command format. No arguments expected after 'clear'."
        continue
      fi
      ;;
    *)
      echo "Unknown command: $COMMAND"
      continue
      ;;
  esac

  echo "$INPUT" > "$FIFO_PATH"
  echo "Command sent: $INPUT"
done

