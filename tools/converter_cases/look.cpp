#include "look.h"
namespace cases {
Look& look() { static Look l; return l; }
}
