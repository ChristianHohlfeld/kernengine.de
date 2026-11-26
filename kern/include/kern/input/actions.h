// Input actions (kern/docs/app.md 4.): the project names what its player does - "jump", "moveX" - and binds keys,
// gamepad buttons and sticks, mouse buttons and touch to it; the game asks for the action by its name
// (Engine::action, Engine::axis), never for a key. The bindings are data of the project (settings "input"), the
// editor shows them in the project's settings.
#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "kern/core/json.h"

namespace kern {

/// Keyboard keys (KeyboardEvent.code order in kern/web/app.mjs KEY_CODES).
enum class KERN_BLUEPRINT_TYPE Key : u8 {
  unknown = 0,
  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
  digit0, digit1, digit2, digit3, digit4, digit5, digit6, digit7, digit8, digit9,
  left, right, up, down, space, enter, escape, tab, backspace, shift_left, shift_right, control_left, control_right, alt_left, alt_right,
  count
};


/// The buttons of a gamepad in the browser's standard mapping (index = the button's number there).
enum class PadButton : u8 { a, b, x, y, lb, rb, lt, rt, back, start, ls, rs, up, down, left, right, home, count };
/// A stick's direction of a gamepad (up and right are +1).
enum class PadAxis : u8 { none, left_x, left_y, right_x, right_y, triggers, count };
/// Where a finger holds an action: none, the left or the right half of the screen, anywhere.
enum class TouchZone : u8 { none, left, right, anywhere, count };
/// A touch stick: a finger that began on the left half of the screen, its way from where it began (up and right +1;
/// one stick radius - kTouchStickShare of the screen's shorter side - is 1).
enum class TouchAxis : u8 { none, stick_x, stick_y, count };
constexpr f32 kTouchStickShare = 0.125f;

struct ActionBinding {
  std::string name;
  std::vector<Key> keys;
  std::vector<PadButton> pad;
  std::vector<u32> mouse;     // mouse buttons (0 left, 1 middle, 2 right)
  TouchZone touch = TouchZone::none;
};

struct AxisBinding {
  std::string name;
  std::vector<Key> negative, positive;   // keys: -1 and +1 (both held: 0)
  PadAxis pad = PadAxis::none;
  f32 deadzone = 0.2f;                   // of the pad's stick: below it 0, above it scaled back to 0..1
  TouchAxis touch = TouchAxis::none;
};

/// The project's actions and axes (ProjectSettings::input).
struct InputActions {
  std::vector<ActionBinding> actions;
  std::vector<AxisBinding> axes;
  const ActionBinding* action(std::string_view name) const {
    for (const ActionBinding& a : actions)
      if (a.name == name) return &a;
    return nullptr;
  }
  const AxisBinding* axis(std::string_view name) const {
    for (const AxisBinding& a : axes)
      if (a.name == name) return &a;
    return nullptr;
  }
};

/// The names in the project file (keys: "a".."z", "digit0".."digit9", "left", "right", "up", "down", "space",
/// "enter", "escape", "tab", "backspace", "shiftLeft", "shiftRight", "controlLeft", "controlRight", "altLeft",
/// "altRight"; pad buttons: "a", "b", "x", "y", "lb", "rb", "lt", "rt", "back", "start", "ls", "rs", "up", "down",
/// "left", "right", "home"; pad axes: "leftX", "leftY", "rightX", "rightY", "triggers"; touch: "left", "right",
/// "anywhere", "stickX", "stickY").
const char* key_name(Key k);
bool parse_key(std::string_view name, Key& out);
/// The key of a KeyboardEvent.code ("KeyW", "ArrowLeft", "Space" - the order of kern/web/app.mjs KEY_CODES): what a host
/// that gets the browser's keys hands Input::key (the editor's Play Project). False: no key of the engine has that code.
bool key_of_code(std::string_view code, Key& out);

json::Value input_to_json(const InputActions& input);
/// Reads the member "input" of the settings. False - `error` says where (input.actions[1].keys[0] ...) and why: a key
/// none of the object's (with the keys there are), a name none of the list (with the names there are), an action or
/// axis without a name or with a name twice (actions and axes share their names).
bool input_from_json(const json::Value& v, InputActions& out, std::string* error);

}  // namespace kern
