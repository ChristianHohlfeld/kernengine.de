#include "ownget.h"
namespace cases {
Holder& holder() { static Holder h; return h; }
}
