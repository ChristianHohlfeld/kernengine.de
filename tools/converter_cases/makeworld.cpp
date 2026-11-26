#include "makeworld.h"
namespace sc {
Maker& maker() { static Maker m; return m; }
}
