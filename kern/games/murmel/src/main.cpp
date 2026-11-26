// Der Einstieg von Murmel (kern/docs/project.md 4.): das eingebettete Projekt (kern/tools/embed_project.py) - seine
// Einstellungen, seine Levels und sein Programm. Die App ist der Programm-Blueprint Murmel des Projekts: der Konverter
// schreibt ihn aus der Quelle src/murmel.cpp (tests/convert.sh --write), das Spiel baut ihn als seine C++-Klasse
// (build.sh: --project --classes) - die Quelle selbst ist nicht im Spiel.
#include "kern/app/run.h"
#include "kern/game/embedded.h"
#include "kern/game/program.h"

int kern_main() {
  return kern::run(kern::game::embedded_project());
}
