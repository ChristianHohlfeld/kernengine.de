#include "actors.h"
namespace cases {
Crowd& crowd() { static Crowd c; return c; }
}
