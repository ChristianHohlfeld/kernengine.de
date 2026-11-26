#include "getter.h"
namespace sc {
Holder& holder() { static Holder h; return h; }
void take_pad(kern::i32 connected, kern::f32 lx, kern::u32 buttons) {
  PadState& p = holder().pad;
  p.pressed = buttons & ~p.buttons;
  p.connected = connected != 0;
  p.lx = lx;
  p.buttons = buttons;
}
kern::f32 doubled(kern::f32 x) { kern::f32 y = x; y *= 2; return y; }
}
namespace sc {
Other& other() { static Other o; return o; }
void Other::look() { seen = holder().pad.lx; }
}
