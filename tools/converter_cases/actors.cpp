#include "actors.h"
namespace sc {
Crowd& crowd() { static Crowd c; return c; }
}
