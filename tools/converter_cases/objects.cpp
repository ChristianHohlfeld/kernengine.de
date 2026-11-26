#include "objects.h"
namespace sc {
Board& board() { static Board b; return b; }
}
