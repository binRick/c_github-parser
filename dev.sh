#!/bin/bash
set -eou pipefail
reset
CMD="meson b --reconfigure && ninja -C b && clear && ./test.sh"
EXEC="sh -- -c \"clear;${@:-$CMD}||true\""
cmd="passh -L .nodemon reap nodemon \
    -w . -i b -i dev -i deps -V \
    --delay .1 \
    -i Makefile \
    -w src \
    -w include \
    -e c,h,sh,build \
    --signal SIGKILL \
    -x $EXEC"

eval "$cmd"
