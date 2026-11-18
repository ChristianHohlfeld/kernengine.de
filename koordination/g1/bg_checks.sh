#!/usr/bin/env bash
S=/home/claude/kernengine.de/koordination
P="a pointer|tuple / variant|list: erase()|an assignment to|copied or moved as a whole|made by a constructor|not made by a constructor of it|where a Int is needed|handed to change to a function|break in the middle|a member of Int is set"
$S/g1/check.sh k1 "$P" src/game.cpp src/game_guided.cpp src/game_controls.cpp
$S/g1/check.sh k2 "$P" src/game.cpp src/game_look.cpp src/game_gaps.cpp
$S/g1/check.sh k3 "$P" src/game.cpp src/game_ui.cpp src/game_vr.cpp
