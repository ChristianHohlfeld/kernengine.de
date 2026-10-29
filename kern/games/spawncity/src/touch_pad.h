// Touch controls for the vehicles (SpawnCity shared_sticks.mjs / thumb_layout.mjs /
// edge_hud_layout.mjs): two 168 px sticks flush with the safe corners that every vehicle "takes
// over", plus the context buttons of the mode packed around the thumbs. On foot Kern's own
// TouchControls (the soldier's layout) runs and this pad only adds the mode buttons. The page draws
// the view with Kern's TouchOverlay (kern/web/studio/game.mjs).
#pragma once

#include <string>
#include <vector>

#include "common.h"
#include "kern/game/touch.h"

namespace sc {

struct PadButton {
  std::string id, label;
  u32 color = 0x0d1118;
  bool on = false;
};

class TouchPad {
 public:
  void set_frame(f32 w, f32 h, f32 sl, f32 st, f32 sr, f32 sb);
  /// Layout for a vehicle (sticks + buttons) or extra buttons only (on foot: sticks = false).
  void set_layout(bool sticks, const std::vector<PadButton>& buttons, const char* left_label = "", const char* right_label = "");
  /// type 0 down, 1 move, 2 up, 3 cancel. True when the pad used the finger.
  bool pointer(u32 type, i32 id, f32 x, f32 y);
  void release_all();
  Vec2 left() const { return left_; }     // -1..1, y up positive
  Vec2 right() const { return right_; }
  bool held(const std::string& id) const;
  /// Pressed since the last call (edge), then cleared.
  bool take_pressed(const std::string& id);
  /// The overlay JSON; `base` (Kern's soldier view) contributes sticks / buttons on foot.
  std::string view_json(const kern::game::TouchView* base) const;
  bool sticks() const { return sticks_; }

 private:
  struct Btn { PadButton b; f32 x, y, w, h; bool pressed = false; };
  struct Finger { i32 id; int kind; std::string button; f32 x0, y0; };  // kind 0 left, 1 right, 2 button
  void layout();
  f32 w_ = 0, h_ = 0, sl_ = 0, st_ = 0, sr_ = 0, sb_ = 0;
  bool sticks_ = false;
  std::vector<PadButton> wanted_;
  std::vector<Btn> buttons_;
  std::vector<Finger> fingers_;
  std::vector<std::string> pressed_;
  std::string left_label_, right_label_;
  Vec2 left_, right_;
  f32 size_ = 168;
};

}  // namespace sc
