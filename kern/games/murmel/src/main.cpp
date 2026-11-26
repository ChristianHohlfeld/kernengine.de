// Der Einstieg von Murmel, wenn die App die C++-Quelle ist (kern/docs/project.md 4.): die Einstellungen kommen aus
// dem eingebetteten Projekt (kern/tools/embed_project.py), die App ist murmel::Murmel.
#include "kern/app/run.h"
#include "kern/game/embedded.h"
#include "murmel.h"

int kern_main() {
  static murmel::Murmel spiel;
  return kern::run(spiel, kern::game::embedded_project().settings);
}
