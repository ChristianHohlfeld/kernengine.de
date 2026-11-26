// Der Einstieg von Murmel, wenn die App die C++-Quelle ist (kern/docs/project.md 4.): die Einstellungen kommen aus
// dem eingebetteten Projekt (kern/tools/embed_project.py), die App ist das eine Objekt murmel::spiel().
#include "kern/app/run.h"
#include "kern/game/embedded.h"
#include "murmel.h"

int kern_main() {
  return kern::run(murmel::spiel(), kern::game::embedded_project().settings);
}
