#include "makeworld.h"
namespace cases {
Maker& maker() { static Maker m; return m; }
}
