#include "ownget.h"
namespace sc {
Holder& holder() { static Holder h; return h; }
}
