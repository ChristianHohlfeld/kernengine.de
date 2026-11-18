#!/usr/bin/env bash
# minitrace.sh <regex>: mini.sh with the trace
S=/home/claude/kernengine.de/koordination
TRACE=$S/g2/trace.py RX="$1" $S/g2/mini.sh
