#!/usr/bin/env bash
# minitrace.sh <regex>: mini.sh with the trace
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
TRACE=$S/g2/trace.py RX="$1" $S/g2/mini.sh
