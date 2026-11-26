// Input actions: names of keys, pad buttons and axes, touch, and the "input" member of a project's settings
// (kern/input/actions.h, kern/docs/app.md 4.).
#include "kern/input/actions.h"

#include "kern/app/app.h"

namespace kern {
namespace {

constexpr const char* kKeyNames[] = {
    "unknown", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
    "v", "w", "x", "y", "z", "digit0", "digit1", "digit2", "digit3", "digit4", "digit5", "digit6", "digit7", "digit8",
    "digit9", "left", "right", "up", "down", "space", "enter", "escape", "tab", "backspace", "shiftLeft", "shiftRight",
    "controlLeft", "controlRight", "altLeft", "altRight"};
static_assert(sizeof kKeyNames / sizeof kKeyNames[0] == static_cast<std::size_t>(Key::count), "a name for every key");
// (KeyboardEvent.code of each key, in the order of Key - kern/web/app.mjs KEY_CODES)
constexpr const char* kKeyCodes[] = {
    "", "KeyA", "KeyB", "KeyC", "KeyD", "KeyE", "KeyF", "KeyG", "KeyH", "KeyI", "KeyJ", "KeyK", "KeyL", "KeyM", "KeyN",
    "KeyO", "KeyP", "KeyQ", "KeyR", "KeyS", "KeyT", "KeyU", "KeyV", "KeyW", "KeyX", "KeyY", "KeyZ", "Digit0", "Digit1",
    "Digit2", "Digit3", "Digit4", "Digit5", "Digit6", "Digit7", "Digit8", "Digit9", "ArrowLeft", "ArrowRight", "ArrowUp",
    "ArrowDown", "Space", "Enter", "Escape", "Tab", "Backspace", "ShiftLeft", "ShiftRight", "ControlLeft", "ControlRight",
    "AltLeft", "AltRight"};
static_assert(sizeof kKeyCodes / sizeof kKeyCodes[0] == static_cast<std::size_t>(Key::count), "a code for every key");
constexpr const char* kPadButtonNames[] = {"a", "b", "x", "y", "lb", "rb", "lt", "rt", "back", "start", "ls", "rs", "up", "down", "left", "right", "home"};
static_assert(sizeof kPadButtonNames / sizeof kPadButtonNames[0] == static_cast<std::size_t>(PadButton::count));
constexpr const char* kPadAxisNames[] = {"none", "leftX", "leftY", "rightX", "rightY", "triggers"};
static_assert(sizeof kPadAxisNames / sizeof kPadAxisNames[0] == static_cast<std::size_t>(PadAxis::count));
constexpr const char* kTouchZoneNames[] = {"none", "left", "right", "anywhere"};
static_assert(sizeof kTouchZoneNames / sizeof kTouchZoneNames[0] == static_cast<std::size_t>(TouchZone::count));
constexpr const char* kTouchAxisNames[] = {"none", "stickX", "stickY"};
static_assert(sizeof kTouchAxisNames / sizeof kTouchAxisNames[0] == static_cast<std::size_t>(TouchAxis::count));

template <class E, std::size_t N>
bool parse_name(std::string_view s, const char* const (&names)[N], E& out, std::size_t from = 0) {
  for (std::size_t i = from; i < N; ++i)
    if (s == names[i]) { out = static_cast<E>(i); return true; }
  return false;
}
template <std::size_t N>
std::string listed(const char* const (&names)[N], std::size_t from = 0) {
  std::string s;
  for (std::size_t i = from; i < N; ++i) s += (s.empty() ? "" : ", ") + std::string(names[i]);
  return s;
}

struct Read {
  std::string* error;
  bool fail(const std::string& why) {
    if (error) *error = why;
    return false;
  }
  bool keys(const json::Value& v, const std::string& at, std::initializer_list<std::string_view> known) {
    if (!v.is_object()) return fail(at + ": must be an object");
    const std::string e = json::unknown_key(v, known);
    return e.empty() || fail(at + "." + e);
  }
  /// A list of names of the table `names` (from index `from`).
  template <class E, std::size_t N>
  bool list(const json::Value& v, const char* key, const std::string& at, const char* const (&names)[N], std::vector<E>& out, std::size_t from = 0) {
    const json::Value* x = v.find(key);
    if (!x) return true;
    if (!x->is_array()) return fail(at + "." + key + ": must be a list of names");
    for (std::size_t i = 0; i < x->size(); ++i) {
      E e{};
      const json::Value& n = (*x)[i];
      if (!n.is_string() || !parse_name(n.as_string(), names, e, from))
        return fail(at + "." + key + "[" + std::to_string(i) + "]: unknown name " + (n.is_string() ? "\"" + n.as_string() + "\"" : std::string("(not a text)")) +
                    " - one of " + listed(names, from));
      out.push_back(e);
    }
    return true;
  }
  template <class E, std::size_t N>
  bool one(const json::Value& v, const char* key, const std::string& at, const char* const (&names)[N], E& out) {
    const json::Value* x = v.find(key);
    if (!x) return true;
    if (!x->is_string() || !parse_name(x->as_string(), names, out))
      return fail(at + "." + key + ": unknown name " + (x->is_string() ? "\"" + x->as_string() + "\"" : std::string("(not a text)")) + " - one of " + listed(names));
    return true;
  }
  bool name(const json::Value& v, const std::string& at, std::string& out, const InputActions& so_far) {
    if (!v["name"].is_string() || v["name"].as_string().empty()) return fail(at + ": has no name");
    out = v["name"].as_string();
    if (so_far.action(out) || so_far.axis(out)) return fail(at + " \"" + out + "\": the name stands twice (actions and axes share their names)");
    return true;
  }
};

template <class E, std::size_t N>
json::Value names_of(const std::vector<E>& v, const char* const (&names)[N]) {
  json::Value a = json::Value::array();
  for (E e : v) a.push(names[static_cast<std::size_t>(e)]);
  return a;
}

}  // namespace

const char* key_name(Key k) { return static_cast<u32>(k) < static_cast<u32>(Key::count) ? kKeyNames[static_cast<u32>(k)] : "unknown"; }
bool parse_key(std::string_view name, Key& out) { return parse_name(name, kKeyNames, out, 1); }
bool key_of_code(std::string_view code, Key& out) {
  for (u32 k = 1; k < static_cast<u32>(Key::count); ++k)
    if (code == kKeyCodes[k]) {
      out = static_cast<Key>(k);
      return true;
    }
  return false;
}

json::Value input_to_json(const InputActions& input) {
  json::Value o = json::Value::object();
  json::Value actions = json::Value::array();
  for (const ActionBinding& a : input.actions) {
    json::Value x = json::Value::object();
    x.set("name", a.name);
    if (!a.keys.empty()) x.set("keys", names_of(a.keys, kKeyNames));
    if (!a.pad.empty()) x.set("pad", names_of(a.pad, kPadButtonNames));
    if (!a.mouse.empty()) {
      json::Value m = json::Value::array();
      for (u32 b : a.mouse) m.push(b);
      x.set("mouse", std::move(m));
    }
    if (a.touch != TouchZone::none) x.set("touch", kTouchZoneNames[static_cast<u32>(a.touch)]);
    actions.push(std::move(x));
  }
  o.set("actions", std::move(actions));
  json::Value axes = json::Value::array();
  for (const AxisBinding& a : input.axes) {
    json::Value x = json::Value::object();
    x.set("name", a.name);
    if (!a.negative.empty()) x.set("negative", names_of(a.negative, kKeyNames));
    if (!a.positive.empty()) x.set("positive", names_of(a.positive, kKeyNames));
    if (a.pad != PadAxis::none) x.set("pad", kPadAxisNames[static_cast<u32>(a.pad)]);
    x.set("deadzone", static_cast<f64>(a.deadzone));
    if (a.touch != TouchAxis::none) x.set("touch", kTouchAxisNames[static_cast<u32>(a.touch)]);
    axes.push(std::move(x));
  }
  o.set("axes", std::move(axes));
  return o;
}

bool input_from_json(const json::Value& v, InputActions& out, std::string* error) {
  Read r{error};
  InputActions in;
  if (!r.keys(v, "input", {"actions", "axes"})) return false;
  if (const json::Value* list = v.find("actions")) {
    if (!list->is_array()) return r.fail("input.actions: must be a list");
    for (std::size_t i = 0; i < list->size(); ++i) {
      const json::Value& x = (*list)[i];
      const std::string at = "input.actions[" + std::to_string(i) + "]";
      ActionBinding a;
      if (!r.keys(x, at, {"name", "keys", "pad", "mouse", "touch"}) || !r.name(x, at, a.name, in) ||
          !r.list(x, "keys", at, kKeyNames, a.keys, 1) || !r.list(x, "pad", at, kPadButtonNames, a.pad) || !r.one(x, "touch", at, kTouchZoneNames, a.touch))
        return false;
      if (const json::Value* m = x.find("mouse")) {
        if (!m->is_array()) return r.fail(at + ".mouse: must be a list of buttons (0 left, 1 middle, 2 right)");
        for (std::size_t j = 0; j < m->size(); ++j) {
          const f64 b = (*m)[j].as_number(-1);
          if (!(b == 0 || b == 1 || b == 2)) return r.fail(at + ".mouse[" + std::to_string(j) + "]: a mouse button is 0 (left), 1 (middle) or 2 (right)");
          a.mouse.push_back(static_cast<u32>(b));
        }
      }
      in.actions.push_back(std::move(a));
    }
  }
  if (const json::Value* list = v.find("axes")) {
    if (!list->is_array()) return r.fail("input.axes: must be a list");
    for (std::size_t i = 0; i < list->size(); ++i) {
      const json::Value& x = (*list)[i];
      const std::string at = "input.axes[" + std::to_string(i) + "]";
      AxisBinding a;
      if (!r.keys(x, at, {"name", "negative", "positive", "pad", "deadzone", "touch"}) || !r.name(x, at, a.name, in) ||
          !r.list(x, "negative", at, kKeyNames, a.negative, 1) || !r.list(x, "positive", at, kKeyNames, a.positive, 1) ||
          !r.one(x, "pad", at, kPadAxisNames, a.pad) || !r.one(x, "touch", at, kTouchAxisNames, a.touch))
        return false;
      if (const json::Value* d = x.find("deadzone")) {
        const f64 z = d->as_number(-1);
        if (!(z >= 0 && z < 1)) return r.fail(at + ".deadzone: from 0 to under 1");
        a.deadzone = static_cast<f32>(z);
      }
      in.axes.push_back(std::move(a));
    }
  }
  out = std::move(in);
  return true;
}

}  // namespace kern
