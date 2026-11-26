#include "lookapp.h"
namespace cases {
LookApp& look_app() { static LookApp a; return a; }
}
