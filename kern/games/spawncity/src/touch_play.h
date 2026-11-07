// SpawnCity's touch controls for every mode, in the game folder (Kern's TouchControls knows only
// its own soldier layout): one pair of 168 px corner sticks for everything (thumb_layout.mjs,
// shared_sticks.mjs), deflection from where the thumb lands (control_semantics.mjs normalizedPointer),
// the on-foot finger ownership of foot_touch_controller.mjs (buttons > sticks > the screen; the
// screen fires at the finger), the drone's altitude pad (simulator.mjs), the jet's hold-to-eject
// (jet_mode.mjs), pinch zoom in the car (player_car_mode.mjs) and the button placement of
// edge_hud_layout.mjs. No GameWorld: the game reads sticks, fingers and button edges each frame,
// the page draws view_json() with Kern's TouchOverlay. Coordinates: CSS px of the viewport.
#pragma once
#include <algorithm>

#include <string>
#include <vector>

#include "common.h"

namespace sc {

enum class TouchMode : u8 { off, foot, drone, car, jet, chute };

/// What the layout shows (the game fills it every frame).
struct TouchLayout {
  TouchMode mode = TouchMode::off;
  bool alive = true;
  std::string weapon;              // name on the thumb weapon button
  bool sniper = false, scope_on = false;
  bool grenade = false;            // hand grenade available (on foot)
  i32 grenades = 0;
  f32 grenade_left = -1;           // fuse left while cooking (s), < 0 not cooking
  bool near_car = false, near_jet = false, emp = false;
  bool strike_ready = true, aiming = false;
  bool armed = false;              // drone
  bool cockpit = false;            // car camera (label INNEN / AUSSEN)
  bool hover = false, rocket = false, ground = false;  // jet: hover mode, rocket selected, on its gear
  bool drone_cooldown = false;     // the mode button shows DRONE while a new drone is not ready
  bool pad_drone = false;          // XBOX CONTROLLER on: no drone sticks, altitude pad, ARM / STOP (simulator.mjs)
};

/// One button of the overlay.
struct TouchBtn {
  std::string id, label;
  f32 x = 0, y = 0, w = 0, h = 0;
  bool round = true, pressed = false, on = false;
  bool on_down = true;             // acts on pointerdown (else on a click: up inside)
  u32 color = 0x0d1118;
};

struct TouchFire {
  i32 id = 0;
  f32 x = 0, y = 0;                // where the finger is now (aim point)
  f32 down_t = 0;                  // s (page clock)
  f32 last_shot = -1;              // set by the game (repeat fire)
  bool fresh = true;               // went down since the game last looked
  i32 hand = 0;                    // fists: 0 right half, 1 left half (foot_touch_controller.mjs)
  f32 depth = -1;                  // the aim depth along its screen ray, resolved when it went down (m)
  bool lifted = false;             // let go before the game saw it: kept for one read (a quick tap)
};

class TouchPlay {
 public:
  static constexpr f32 kStick = 168;          // thumb_layout.mjs STICK
  static constexpr f32 kRightInset = 76;      // Kern: the right stick off the edge (room for the thumb to turn, as CoD Mobile), in every mode
  f32 right_inset() const { return kRightInset; }
  static constexpr f32 kRadiusK = 0.42f;      // control_semantics.mjs normalizedPointer
  static constexpr f32 kEdgeStart = 0.92f, kEdgeRampPx = 42;  // foot_touch_controller.mjs edge turn
  static constexpr f32 kEjectHold = 0.65f;    // jet_mode.mjs: EJECT after 650 ms

  void set_frame(f32 w, f32 h, f32 sl, f32 st, f32 sr, f32 sb);
  void set_layout(const TouchLayout& l);
  /// type 0 down, 1 move, 2 up, 3 cancel; t in ms. True when the controls own the finger (a finger
  /// they do not own - car pinch, the drone's tap fire, free screen - may also be a long-press ping).
  bool pointer(u32 type, i32 id, f32 x, f32 y, f64 t_ms);
  void release_all();

  // ---- what the fingers give (read by the game each frame) ----
  Vec2 move() const { return move_; }           // left stick, x right, y down, unit disc
  Vec2 look() const { return look_; }           // right stick (vehicles, drone), same axes
  bool move_live() const { return move_id_ >= 0; }
  bool look_live() const { return look_id_ >= 0; }
  /// On foot: the look stick's edge turn (x/m * edge, y/m * edge), 0 when inside the ring.
  Vec2 look_edge() const { return look_edge_; }
  /// On foot: the look drag since the last call (rad; yaw right-positive, pitch up-positive).
  Vec2 take_drag();
  /// Sprint (pushed to the rim forwards: m >= .90 and y < -.35).
  bool sprint() const;
  std::vector<TouchFire>& fires() { return fires_; }
  /// After the game's frame read the fingers: the taps let go in between are gone now.
  void drop_lifted() { fires_.erase(std::remove_if(fires_.begin(), fires_.end(), [](const TouchFire& f) { return f.lifted; }), fires_.end()); }
  /// Drone altitude pad axis (-1..1, up positive), 0 when released.
  f32 clearance() const { return clearance_; }
  /// Grenade button held: drag up in px since it went down.
  f32 grenade_drag() const { return nade_up_; }
  /// Car: pinch factor since the last call (1 = none; < 1 closer).
  f32 take_zoom();
  bool held(const std::string& id) const;
  bool take_pressed(const std::string& id);
  /// A click on the page's copy of a button (mouse, keyboard focus): the same press as a finger.
  void inject(const std::string& id) { pressed_.push_back(id); }
  bool take_released(const std::string& id);
  /// A tap on the free screen while aiming the air strike (fighter_jets.mjs overlay).
  bool take_strike_tap(f32& x, f32& y);
  bool any() const { return !fingers_.empty(); }

  /// The rectangles of the buttons as the page drew them (CSS px in the canvas frame): fingers hit
  /// what the player sees. Ids as in view_json ("clearance" = the drone's altitude pad).
  struct DomRect { std::string id; f32 x = 0, y = 0, w = 0, h = 0; };
  void set_dom_rects(std::vector<DomRect> rects);

  std::string view_json() const;
  const std::vector<TouchBtn>& buttons() const { return buttons_; }
  TouchMode mode() const { return lay_.mode; }

 private:
  struct Finger {
    i32 id = 0;
    enum Kind : u8 { move, look, fire, press, clearance, pinch, strike, none } kind = none;
    std::string button;
    f32 x0 = 0, y0 = 0, x = 0, y = 0, last_x = 0, last_y = 0;
    f64 t0 = 0, last_t = 0;
    Vec2 base;
  };
  void layout();
  void apply_dom_rects();
  bool no_sticks() const { return lay_.aiming || (lay_.mode == TouchMode::drone && lay_.pad_drone); }
  TouchBtn* button_at(f32 x, f32 y);
  bool in_circle(f32 cx, f32 cy, f32 r, f32 x, f32 y) const;
  void place_row(std::vector<TouchBtn>& items, f32 x0, f32 bottom, f32 dir, f32 max_w);
  void add(const TouchBtn& b) { buttons_.push_back(b); }

  f32 w_ = 960, h_ = 540, sl_ = 0, st_ = 0, sr_ = 0, sb_ = 0;
  TouchLayout lay_;
  std::vector<TouchBtn> buttons_;
  std::vector<Finger> fingers_;
  std::vector<std::string> pressed_, released_;
  std::vector<TouchFire> fires_;
  i32 move_id_ = -1, look_id_ = -1;
  Vec2 move_, look_, look_edge_, drag_;
  f32 clearance_ = 0, nade_up_ = 0;
  f32 zoom_ = 1, pinch_d_ = 0;
  bool strike_tap_ = false;
  std::vector<DomRect> dom_;
  f32 strike_x_ = 0, strike_y_ = 0;
  // the altitude pad's rectangle (drone)
  f32 pad_x_ = 0, pad_y_ = 0, pad_w_ = 0, pad_h_ = 0;
};

}  // namespace sc
