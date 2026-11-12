// Kern Logic: the gameplay nodes as the library knows them - their names, pins and settings.
//
// This file needs nothing of the game: the editor, the C++ notation and the tools that write a
// level's Blueprints as classes (tools/blueprint_cpp) know the nodes from here. What a node does
// needs the game and is bound to it in logic_nodes.cpp (kern::game::nodes).
#include <cctype>
#include <string>
#include <vector>

#include "kern/game/blueprint_nodes.h"
#include "kern/game/logic_nodes.h"
#include "kern/logic/logic.h"

namespace kern::game {

namespace {

using logic::NodeDef;
using logic::NodeKind;
using logic::ParamDef;
using logic::PinDef;
using logic::PinType;

constexpr u32 kEventColor = 0xb4232f;

PinDef exec_in(std::string id = "exec", std::string label = "") { return {std::move(id), std::move(label), PinType::exec, {}}; }
PinDef exec_out(std::string id = "then", std::string label = "") { return {std::move(id), std::move(label), PinType::exec, {}}; }
PinDef data(std::string id, std::string label, PinType t, json::Value def = {}) { return {std::move(id), std::move(label), t, std::move(def)}; }
json::Value vec3(f32 x, f32 y, f32 z) {
  json::Value a = json::Value::array();
  a.push(static_cast<f64>(x));
  a.push(static_cast<f64>(y));
  a.push(static_cast<f64>(z));
  return a;
}
ParamDef param(std::string key, std::string label, ParamDef::Kind kind, std::string def = {}, std::vector<std::string> choices = {}) {
  ParamDef p;
  p.key = std::move(key);
  p.label = std::move(label);
  p.kind = kind;
  p.def = std::move(def);
  p.choices = std::move(choices);
  return p;
}
NodeDef make(std::string type, std::string label, std::string category, NodeKind kind, std::string help, std::string keywords = {}) {
  NodeDef d;
  d.type = std::move(type);
  d.label = std::move(label);
  d.category = std::move(category);
  d.kind = kind;
  d.help = std::move(help);
  d.keywords = std::move(keywords);
  return d;
}
/// A step: exec in -> what it does -> exec out. (Every node here is a function of a Blueprint's
/// C++ - blueprint_nodes.h - or a handler: native.)
NodeDef step(std::string type, std::string label, std::string category, std::vector<PinDef> in, std::vector<PinDef> out, std::string help, std::string keywords = {}) {
  NodeDef d = make(std::move(type), std::move(label), std::move(category), NodeKind::action, std::move(help), std::move(keywords));
  d.inputs = {exec_in()};
  for (PinDef& p : in) d.inputs.push_back(std::move(p));
  d.outputs = {exec_out()};
  for (PinDef& p : out) d.outputs.push_back(std::move(p));
  d.native = true;
  return d;
}
/// A value read from the world.
NodeDef value(std::string type, std::string label, std::string category, std::vector<PinDef> in, std::vector<PinDef> out, std::string help, std::string keywords = {}) {
  NodeDef d = make(std::move(type), std::move(label), std::move(category), NodeKind::pure, std::move(help), std::move(keywords));
  d.inputs = std::move(in);
  d.outputs = std::move(out);
  d.native = true;
  return d;
}
/// An event node: `channel` is the GameWorld event type; its outputs are the event's values.
NodeDef event(std::string type, std::string label, std::string channel, std::vector<PinDef> out, std::string help, std::string keywords = {}) {
  NodeDef d = make(std::move(type), std::move(label), "Events", NodeKind::event, std::move(help), std::move(keywords));
  d.event = std::move(channel);
  d.outputs = {exec_out()};
  for (PinDef& p : out) d.outputs.push_back(std::move(p));
  d.color = kEventColor;
  d.accepts = [](const logic::Node&, const logic::Event&) { return true; };
  d.native = true;  // (a handler of the Blueprint's class, like every event)
  return d;
}

std::string key_label(const std::string& code) {
  if (code.rfind("Key", 0) == 0 && code.size() == 4) return code.substr(3);
  if (code.rfind("Digit", 0) == 0 && code.size() == 6) return code.substr(5);
  return code;
}

/// "F" -> "KeyF", "1" -> "Digit1", "Space" stays.
std::string key_code(std::string k) {
  if (k.size() == 1 && std::isalpha(static_cast<unsigned char>(k[0]))) return std::string("Key") + static_cast<char>(std::toupper(static_cast<unsigned char>(k[0])));
  if (k.size() == 1 && std::isdigit(static_cast<unsigned char>(k[0]))) return "Digit" + k;
  return k;
}

// (a function: the library is filled while the program starts, before this file's own constants may be there)
std::vector<std::string> actions() { return {"jump", "Jump", "fire", "Fire", "aim", "Aim", "reload", "Reload", "crouch", "Crouch", "interact", "Interact", "boost", "Sprint / boost"}; }

void add_events(logic::Library& lib) {
  const PinDef actor_out = data("actor", "Actor", PinType::actor), other_out = data("other", "Other Actor", PinType::actor);
  lib.add(event("game.overlap_begin", "On Begin Overlap", "game.overlap_begin", {actor_out, other_out},
                "Something entered this actor's sensor collider (a trigger volume). Other Actor is who came in.", "trigger enter touch volume"));
  lib.add(event("game.overlap_end", "On End Overlap", "game.overlap_end", {actor_out, other_out}, "Something left this actor's sensor collider.", "trigger leave exit"));
  lib.add(event("game.touched_sensor", "On Entered Sensor", "game.overlapped_begin", {actor_out, data("other", "Sensor Actor", PinType::actor)},
                "This actor (e.g. a player) walked into someone else's sensor.", "overlap volume"));
  lib.add(event("game.hit", "On Hit", "game.hit",
                {actor_out, other_out, data("speed", "Impact Speed", PinType::number), data("point", "Point", PinType::vector), data("normal", "Normal", PinType::vector)},
                "This actor's body hit something hard enough (Box3D hit event).", "collide collision bump impact"));
  lib.add(event("game.pickup", "On Collected", "game.pickup", {actor_out, data("other", "Collector", PinType::actor), data("points", "Points", PinType::number)},
                "A pickup was collected.", "pickup orb"));
  lib.add(event("game.trigger", "On Trigger", "game.trigger", {actor_out, data("other", "Player", PinType::actor)}, "A player touched this trigger.", "goal checkpoint"));
  lib.add(event("game.damaged", "On Damaged", "game.damaged",
                {actor_out, data("other", "Attacker", PinType::actor), data("amount", "Damage", PinType::number), data("head", "Headshot", PinType::boolean)},
                "This actor took damage (needs a Health component).", "hurt shot"));
  lib.add(event("game.death", "On Death", "game.death", {actor_out, data("other", "Killer", PinType::actor), data("head", "Headshot", PinType::boolean)},
                "This actor's health reached 0.", "die killed destroyed"));
  lib.add(event("game.respawn", "On Respawn", "game.respawn", {actor_out}, "This actor came back after dying.", "revive"));
  lib.add(event("game.interact", "On Interact", "game.interact", {actor_out, data("other", "Player", PinType::actor), data("point", "Point", PinType::vector)},
                "A player pressed Interact (F / Y on the gamepad) looking at this actor or standing next to it.", "use button press open"));
  lib.add(event("game.pawn_added", "On Player Joined", "game.pawn_added", {data("actor", "Player", PinType::actor)}, "A player joined (multiplayer).", "connect spawn"));
  {
    NodeDef d = event("game.over", "On Game Over", "game.over", {data("won", "Won", PinType::boolean), data("message", "Message", PinType::text)}, "The game was won or lost.", "end win lose");
    lib.add(std::move(d));
  }
  {
    NodeDef d = event("game.input", "On Input Action", "game.input", {data("actor", "Player", PinType::actor), data("pressed", "Pressed", PinType::boolean)},
                      "A player pressed or released an action (works for every player, on every device: keyboard, gamepad, touch).", "button control press");
    d.params = {param("action", "Action", ParamDef::Kind::choice, "interact", actions()),
                param("edge", "When", ParamDef::Kind::choice, "pressed", {"pressed", "Pressed", "released", "Released", "both", "Both"})};
    d.title = [](const NodeDef&, const logic::Node& n) {
      std::string a = n.param("action", "interact");
      if (!a.empty()) a[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(a[0])));
      const std::string e = n.param("edge", "pressed");
      return "On " + a + (e == "released" ? " Released" : e == "both" ? "" : " Pressed");
    };
    d.accepts = [](const logic::Node& n, const logic::Event& e) {
      if (e.key != n.param("action", "interact")) return false;
      const std::string edge = n.param("edge", "pressed");
      const logic::Value* p = e.arg("pressed");
      const bool pressed = p && p->as_bool();
      return edge == "both" || (edge == "pressed") == pressed;
    };
    lib.add(std::move(d));
  }
  {
    NodeDef d = event("game.key", "On Key", "game.key", {data("pressed", "Pressed", PinType::boolean), data("actor", "Player", PinType::actor)},
                      "A key of this computer's keyboard (only the local player; use On Input Action for gamepads and multiplayer).", "keyboard");
    d.params = {param("key", "Key", ParamDef::Kind::key, "F"), param("edge", "When", ParamDef::Kind::choice, "pressed", {"pressed", "Pressed", "released", "Released", "both", "Both"})};
    d.title = [](const NodeDef&, const logic::Node& n) { return "On Key " + key_label(key_code(n.param("key", "F"))); };
    d.accepts = [](const logic::Node& n, const logic::Event& e) {
      if (e.key != key_code(n.param("key", "F"))) return false;
      const std::string edge = n.param("edge", "pressed");
      const logic::Value* p = e.arg("pressed");
      const bool pressed = p && p->as_bool();
      return edge == "both" || (edge == "pressed") == pressed;
    };
    lib.add(std::move(d));
  }
}

void add_actor_nodes(logic::Library& lib) {
  const json::Value zero3 = vec3(0, 0, 0);
  const PinDef actor_in = data("target", "Actor", PinType::actor, 0);
  lib.add(value("actor.self", "Self", "Actors", {}, {data("actor", "Self", PinType::actor)}, "The actor this graph belongs to (none in the level graph).", "this me owner"));
  lib.add(value("actor.player", "Get Player", "Actors", {data("index", "Player Index", PinType::integer, 0)}, {data("actor", "Player", PinType::actor)},
                "The player's actor (0 = this machine's player; 1, 2... other players).", "pawn character me"));
  lib.add(value("actor.player_count", "Player Count", "Actors", {}, {data("count", "Count", PinType::integer)}, "How many players are in the game.", "players number"));
  lib.add(value("actor.find", "Find Actor by Name", "Actors", {data("name", "Name", PinType::text, "Cube")}, {data("actor", "Actor", PinType::actor)},
                "The actor with this name (as in the Outliner).", "get lookup named"));
  lib.add(value("actor.find_tag", "Find Actor with Tag", "Actors", {data("tag", "Tag", PinType::text, "target")}, {data("actor", "Actor", PinType::actor), data("count", "Count", PinType::integer)},
                "The first living actor with the tag, and how many have it.", "get tagged"));
  lib.add(value("actor.all_with_tag", "All Actors with Tag", "Actors", {data("tag", "Tag", PinType::text, "target")}, {data("actors", "Actors", PinType::actor_list)},
                "Every living actor with the tag, as a list (For Each goes through it).", "get tagged every list array"));
  {
    NodeDef d = value("actor.random_tag", "Random Actor with Tag", "Actors", {data("tag", "Tag", PinType::text, "spawn")}, {data("actor", "Actor", PinType::actor)},
                      "One living actor with the tag, chosen at random.", "pick");
    d.memory = true;  // new on every read: the node keeps its sequence
    lib.add(std::move(d));
  }
  lib.add(value("actor.has_tag", "Has Tag", "Actors", {actor_in, data("tag", "Tag", PinType::text, "enemy")}, {data("result", "Result", PinType::boolean)}, "Does the actor carry the tag?", "is"));
  lib.add(value("actor.name", "Get Name", "Actors", {actor_in}, {data("name", "Name", PinType::text)}, "The actor's name.", "display"));
  lib.add(value("actor.valid", "Is Valid", "Actors", {actor_in}, {data("result", "Valid", PinType::boolean)}, "True for an existing, living actor.", "exists alive none null"));
  lib.add(value("actor.location", "Get Location", "Actors", {actor_in}, {data("location", "Location", PinType::vector)}, "World position of the actor.", "position where"));
  lib.add(value("actor.rotation", "Get Rotation", "Actors", {actor_in}, {data("rotation", "Rotation", PinType::vector)}, "World rotation in degrees (X roll, Y pitch, Z yaw).", "orientation angle"));
  lib.add(value("actor.forward", "Get Forward Vector", "Actors", {actor_in}, {data("forward", "Forward", PinType::vector)}, "The direction the actor faces (its X axis).", "direction facing"));
  lib.add(value("actor.velocity", "Get Velocity", "Actors", {actor_in}, {data("velocity", "Velocity", PinType::vector), data("speed", "Speed", PinType::number)}, "Linear velocity (m/s) and speed.",
                "speed motion"));
  lib.add(value("actor.distance", "Distance Between", "Actors", {data("a", "A", PinType::actor, 0), data("b", "B", PinType::actor, 0)}, {data("distance", "Distance", PinType::number)},
                "Metres between two actors (A defaults to Self).", "near far range"));
  lib.add(value("actor.health", "Get Health", "Combat", {actor_in}, {data("health", "Health", PinType::number), data("dead", "Is Dead", PinType::boolean)},
                "Hit points (-1 without a Health component).", "hp life"));
  lib.add(value("game.score", "Get Score", "Game", {}, {data("score", "Score", PinType::number), data("pickups", "Collected", PinType::integer)}, "Score and collected pickups.", "points"));

  // ---- steps ----
  lib.add(step("actor.set_location", "Set Location", "Actors", {actor_in, data("location", "Location", PinType::vector, zero3)}, {}, "Teleports the actor (bodies included).",
               "move teleport position"));
  lib.add(step("actor.set_rotation", "Set Rotation", "Actors", {actor_in, data("rotation", "Rotation", PinType::vector, zero3)}, {}, "Turns the actor (degrees).", "rotate turn orient"));
  lib.add(step("actor.move_by", "Move By", "Actors", {actor_in, data("offset", "Offset", PinType::vector, vec3(0, 0, 1))}, {}, "Moves the actor by an offset.", "translate add offset"));
  lib.add(step("actor.set_velocity", "Set Velocity", "Physics", {actor_in, data("velocity", "Velocity", PinType::vector, zero3)}, {}, "Sets the linear velocity (players too).", "speed launch"));
  lib.add(step("actor.impulse", "Add Impulse", "Physics", {actor_in, data("impulse", "Impulse", PinType::vector, vec3(0, 0, 500))}, {}, "A kick (N·s) to a dynamic body or a player.",
               "push kick throw"));
  lib.add(step("actor.force", "Add Force", "Physics", {actor_in, data("force", "Force", PinType::vector, vec3(0, 0, 1000))}, {},
               "A force (N) for this step; call it every Tick for a steady push.", "thrust wind"));
  lib.add(step("actor.launch", "Launch", "Physics", {actor_in, data("velocity", "Velocity", PinType::vector, vec3(0, 0, 10))}, {},
               "Throws a player or body (keeps its horizontal speed, adds this).", "jump bounce"));
  lib.add(step("actor.simulate", "Set Simulate Physics", "Physics", {actor_in, data("simulate", "Simulate", PinType::boolean, true)}, {}, "Lets the body fall (on) or holds it (off).",
               "dynamic kinematic freeze"));
  lib.add(step("actor.joint_motor", "Set Joint Motor", "Physics",
               {actor_in, data("joint", "Joint Index", PinType::integer, 0), data("enable", "Enable", PinType::boolean, true), data("speed", "Speed (°/s or m/s)", PinType::number, 90.0),
                data("force", "Max Force", PinType::number, 1000.0)},
               {}, "Drives a hinge, slider or rope of the actor (doors, lifts, winches).", "constraint hinge slider drive"));
  lib.add(step("actor.explode", "Explosion", "Physics",
               {data("location", "Location", PinType::vector, zero3), data("radius", "Radius", PinType::number, 6.0), data("impulse", "Strength", PinType::number, 6.0),
                data("damage", "Damage", PinType::number, 50.0)},
               {}, "Box3D radial impulse with flash, sound and damage falling off with distance.", "blast bomb radial"));
  {
    NodeDef d = step("actor.spawn", "Spawn Actor", "Actors",
                     {data("template", "Template", PinType::actor, 0), data("location", "Location", PinType::vector, vec3(0, 0, 1)), data("rotation", "Rotation", PinType::vector, zero3)},
                     {data("spawned", "Spawned", PinType::actor)}, "A copy of a template actor (with its children and logic) - replicated to every player.", "create instantiate clone copy");
    d.params = {param("template", "Template name", ParamDef::Kind::text, "")};
    lib.add(std::move(d));
  }
  lib.add(step("actor.destroy", "Destroy Actor", "Actors", {actor_in}, {}, "Removes the actor for the rest of the game.", "delete remove kill"));
  lib.add(step("actor.enable", "Set Enabled", "Actors", {actor_in, data("enabled", "Enabled", PinType::boolean, false)}, {}, "Hides and disables (or brings back) the actor and its body.",
               "visible hide show active"));
  lib.add(step("actor.set_property", "Set Property", "Actors", {actor_in, data("path", "Property", PinType::text, "components.light.intensity"), data("value", "Value", PinType::text, "20")}, {},
               "Changes any component value (the path as in the level file, e.g. components.mesh.material.color) - replicated.", "component field details"));
  lib.add(step("actor.set_light", "Set Light", "Effects",
               {actor_in, data("color", "Color", PinType::vector, vec3(1, 0.9f, 0.7f)), data("intensity", "Intensity", PinType::number, 10.0), data("range", "Range", PinType::number, 10.0)}, {},
               "Colour (0..1 each), brightness and reach of the actor's point light.", "lamp brightness"));
  lib.add(step("actor.set_color", "Set Color", "Effects", {actor_in, data("color", "Color", PinType::vector, vec3(1, 0.2f, 0.2f))}, {}, "Mesh colour (0..1 each).", "material paint tint"));
}

/// Robots (walkers, the Micro Duck): drive them, look, read their ToF and camera. Commands are in
/// the robot's own units (the real duck's m/s and degrees); distances are world metres.
void add_walker_nodes(logic::Library& lib) {
  const PinDef robot = data("target", "Robot", PinType::actor, 0);
  lib.add(step("walker.move", "Walk", "Robots",
               {robot, data("forward", "Forward (m/s)", PinType::number, 0.3), data("strafe", "Sideways (m/s)", PinType::number, 0.0), data("turn", "Turn (°/s)", PinType::number, 0.0),
                data("duration", "Duration (s, 0 = keep)", PinType::number, 0.0)},
               {},
               "Walks the robot like its gamepad: forward up to 0.4 m/s, sideways 0.3, turning 57 °/s (its trained ranges). Overrides its own behaviour "
               "for the duration (0: until the next Walk; Set Autonomous gives it back).",
               "drive move go duck robot walker twist"));
  lib.add(step("walker.look", "Look", "Robots",
               {robot, data("yaw", "Yaw (°, + left)", PinType::number, 0.0), data("pitch", "Pitch (°, + down)", PinType::number, 0.0), data("duration", "Duration (s, 0 = keep)", PinType::number, 0.0)},
               {}, "Turns the robot's head (its camera and ToF look there): yaw up to 80°, pitch up to 63°.", "head camera gaze duck robot"));
  lib.add(step("walker.autonomous", "Set Autonomous", "Robots", {robot, data("enable", "Autonomous", PinType::boolean, true)}, {},
               "On: the robot roams on its own (camera, ToF, exploration memory) whenever nobody drives it. Off: it stands until driven.", "brain behaviour wander ai duck robot"));
  lib.add(step("walker.skill", "Robot Move", "Robots", {robot, data("move", "Move", PinType::text, "pick")}, {},
               "One of the robot's moves, as its daemon runs them: sit (toggles), pick (ground pick; the crouch on rollers), kick_left, kick_right, roulade; and jump, Kern's own (trained in Kern Lehrer).",
               "skill sit kick roll pick duck robot"));
  lib.add(step("walker.beak", "Robot Beak", "Robots", {robot, data("open", "Open (0-1)", PinType::number, 1.0)}, {}, "Opens (1) or closes (0) the beak.", "mouth jaw duck robot"));
  lib.add(step("walker.pet", "Pet Robot", "Robots", {robot, data("seconds", "Seconds", PinType::number, 3.0)}, {},
               "A hand on the duck for a while: it melts, wiggles or falls asleep, as Pollen's brain does it.", "petting stroke touch duck robot"));
  lib.add(step("walker.hear", "Robot Hears", "Robots", {robot, data("voice", "Voice (else a noise)", PinType::boolean, true)}, {},
               "The duck hears someone talk (it perks up and answers) or a noise (it stirs from a doze).", "sound voice listen duck robot"));
  lib.add(value("walker.holding", "Robot Holds Something", "Robots", {robot}, {data("holding", "Holding", PinType::boolean)},
                "The beak is told to shut and its servo reads it open: something is in it (a sock).", "sock beak carry duck robot"));
  lib.add(step("walker.quack", "Chirp", "Robots", {robot}, {}, "The duck chirps (its own voice, the robot's synth) and opens its beak.", "sound duck robot"));
  lib.add(value("walker.tof", "Robot ToF", "Robots", {robot},
                {data("blocked", "Obstacle Seen", PinType::boolean), data("distance", "Nearest (m)", PinType::number), data("bearing", "Bearing (°, + left)", PinType::number),
                 data("left", "Free Left (0-1)", PinType::number), data("right", "Free Right (0-1)", PinType::number)},
                "The head time-of-flight sensor (8 x 8 zones, 45°): the nearest obstacle with the robot's own floor filter. -1 = nothing in range (4 m real).",
                "distance sensor depth obstacle duck robot"));
  lib.add(value("walker.sees", "Robot Camera Sees", "Robots", {robot, data("other", "Actor", PinType::actor, 0)},
                {data("visible", "Visible", PinType::boolean), data("bearing", "Bearing (°, + left)", PinType::number), data("distance", "Distance (m)", PinType::number)},
                "Whether the actor is in the head camera's picture (62° x 37°) with nothing in between.", "vision see detect camera duck robot"));
  lib.add(value("walker.state", "Robot State", "Robots", {robot},
                {data("state", "Behaviour", PinType::text), data("fallen", "Fallen", PinType::boolean), data("speed", "Speed (m/s)", PinType::number)},
                "What the robot is doing (Chill, LookAround, Wander, TurnInPlace, BackOff, Zoomies, GetUp; Driven / Logic when someone drives it).", "behaviour status fallen duck robot"));
}

void add_game_nodes(logic::Library& lib) {
  const json::Value zero3 = vec3(0, 0, 0);
  const PinDef actor_in = data("target", "Actor", PinType::actor, 0);
  lib.add(step("game.message", "Show Message", "Game", {data("text", "Text", PinType::text, "Hello!")}, {}, "A message in the middle of the screen (every player sees it).", "say hud print notify"));
  lib.add(step("game.add_score", "Add Score", "Game", {data("points", "Points", PinType::number, 10.0)}, {}, "Adds points (wins at the target score when the goal is Score).", "points reward"));
  lib.add(step("game.win", "Win Game", "Game", {data("message", "Message", PinType::text, "You win!")}, {}, "Ends the game as won.", "victory finish complete"));
  lib.add(step("game.lose", "Lose Game", "Game", {data("message", "Message", PinType::text, "Game over")}, {}, "Ends the game as lost.", "fail defeat"));
  {
    NodeDef d = step("game.sound", "Play Sound", "Audio",
                     {data("location", "Location", PinType::vector, zero3), data("volume", "Volume", PinType::number, 1.0), data("pitch", "Pitch", PinType::number, 1.0),
                      data("spatial", "3D", PinType::boolean, true)},
                     {}, "Plays a sound once (3D at the location, or Self's position; or flat when 3D is off).", "audio sfx noise");
    d.params = {param("sound", "Sound", ParamDef::Kind::sound, "builtin:confirm")};
    d.title = [](const NodeDef&, const logic::Node& n) {
      std::string s = n.param("sound", "builtin:confirm");
      if (s.rfind("builtin:", 0) == 0) s = s.substr(8);
      return "Play " + s;
    };
    lib.add(std::move(d));
  }
  lib.add(step("game.particles", "Spawn Sparks", "Effects", {data("location", "Location", PinType::vector, zero3), data("color", "Color", PinType::vector, vec3(1, 0.8f, 0.3f))}, {},
               "A burst of glowing particles.", "fx effect burst confetti"));
  lib.add(step("game.shake", "Camera Shake", "Effects", {data("amount", "Amount", PinType::number, 0.5)}, {}, "Shakes this machine's camera (0..1).", "rumble"));
  lib.add(step("combat.damage", "Apply Damage", "Combat", {actor_in, data("amount", "Damage", PinType::number, 25.0), data("instigator", "Instigator", PinType::actor, 0)}, {},
               "Hurts an actor with a Health component (armor first, death, score).", "hurt hit attack"));
  lib.add(step("combat.heal", "Heal", "Combat", {actor_in, data("amount", "Amount", PinType::number, 25.0)}, {}, "Gives health back (up to the maximum).", "health restore medkit"));
  lib.add(step("combat.kill", "Kill", "Combat", {actor_in}, {}, "Takes all health (respawns like any death).", "die"));
  lib.add(step("combat.ammo", "Give Ammo", "Combat", {data("target", "Player", PinType::actor, 0), data("rounds", "Rounds", PinType::integer, 30)}, {}, "Adds rounds to a soldier's reserve.",
               "bullets reload"));
  // Line trace (a step like Unreal's, so the hit is taken once).
  lib.add(step("physics.trace", "Line Trace", "Physics",
               {data("start", "Start", PinType::vector, zero3), data("end", "End", PinType::vector, vec3(10, 0, 0)), data("ignore", "Ignore", PinType::actor, 0)},
               {data("hit", "Hit", PinType::boolean), data("actor", "Hit Actor", PinType::actor), data("location", "Location", PinType::vector), data("normal", "Normal", PinType::vector)},
               "First thing between Start and End (a Box3D ray).", "raycast ray line of sight"));
  // The same with Box3D's query filter: which collision channels the ray meets, and its own.
  lib.add(step("physics.trace_channels", "Line Trace by Channels", "Physics",
               {data("start", "Start", PinType::vector, zero3), data("end", "End", PinType::vector, vec3(10, 0, 0)), data("ignore", "Ignore", PinType::actor, 0),
                data("mask", "Channels", PinType::integer, 4294967295.0), data("category", "Own Channels", PinType::integer, 4294967295.0)},
               {data("hit", "Hit", PinType::boolean), data("actor", "Hit Actor", PinType::actor), data("location", "Location", PinType::vector), data("normal", "Normal", PinType::vector)},
               "First thing between Start and End that is of one of Channels - the bits of the level's collision channels (the node Channels gives them by name). A collider that does "
               "not collide with Own Channels is left out too: so a collider can ignore certain rays. (Box3D: a ray meets a shape when shape.category & Channels and shape.mask & Own Channels.)",
               "raycast ray line of sight filter category mask layer"));
  lib.add(value("physics.channels", "Channels", "Physics", {data("names", "Names", PinType::text, "World")}, {data("mask", "Channels", PinType::integer)},
                "The bits of the level's collision channels with these names, separated by commas (World Settings: Collision channels). \"All\": every channel. For a collider's "
                "channels and for Line Trace by Channels.",
                "collision channel category mask layer bits filter"));
  // For each actor with a tag.
  {
    NodeDef d = make("flow.for_each_tag", "For Each Actor with Tag", "Flow", NodeKind::flow, "Runs Loop Body for every living actor with the tag, then Completed.", "loop all every");
    d.inputs = {exec_in(), data("tag", "Tag", PinType::text, "target")};
    d.outputs = {exec_out("body", "Loop Body"), data("actor", "Actor", PinType::actor), data("index", "Index", PinType::integer), exec_out("completed", "Completed")};
    d.loop = true;
    d.memory = true;
    d.native = true;
    lib.add(std::move(d));
  }
}

}  // namespace

const std::vector<std::string>& logic_node_types() {
  static const std::vector<std::string> types = [] {
    logic::Library& lib = logic::library();
    const std::size_t before = lib.all().size();
    add_events(lib);
    add_actor_nodes(lib);
    add_game_nodes(lib);
    add_walker_nodes(lib);
    std::vector<std::string> added;
    for (std::size_t i = before; i < lib.all().size(); ++i) added.push_back(lib.all()[i]->type);
    return added;
  }();
  return types;
}

bool logic_node_shapes_registered() { return !logic_node_types().empty(); }

}  // namespace kern::game
