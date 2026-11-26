#include "objects.h"
namespace cases {
Board& board() { static Board b; return b; }
}
