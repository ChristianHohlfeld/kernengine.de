// Kern Logic in the game world: running the level's and the actors' graphs on
// the authority, the events the simulation raises for them, and the actor
// operations their nodes use (with automatic replication of what they change).
#include "kern/logic/engine_libraries.h"
#include <algorithm>
#include "kern/game/project.h"
#include "kern/core/dmath.h"
#include <cmath>

#include "kern/game/geometry.h"
#include "kern/game/runtime.h"
#include "runtime_internal.h"

namespace kern::game {

using rt::kDegree;


namespace {
logic::Value act(ActorId id) { return logic::Value::actor(static_cast<logic::ActorRef>(id)); }
}  // namespace

void GameWorld::start_logic() {
  logic_.reset();
  logic_host_.reset();
  prop_changes_.clear();
  prop_log_.clear();
  spawn_changes_.clear();
  spawn_log_.clear();
  logic_seed_ = 0x9e3779b9u ^ static_cast<u32>(level_.actors.size() * 2654435761u);
  if ((mode_ != SimMode::play && mode_ != SimMode::simulate) || starting_replica_) return;
  bool any = !level_.logic.empty();
  for (const Actor& a : level_.actors) any |= a.logic.has_value() && !a.logic->empty();
  for (const Actor& a : templates_.actors) any |= a.logic.has_value() && !a.logic->empty();   // (what spawn makes runs too)
  if (!any) return;
  logic_host_ = std::make_unique<LogicHost>(*this);
  logic_ = std::make_unique<logic::Runtime>(*logic_host_);
  logic_->native = logic_native;
  // (the libraries a Blueprint uses: the level's, by the name of the actor that has them, then the project's)
  logic_->blueprints = logic::with_engine(world_blueprints(level_, project_));  // (and the engine's own libraries: kern/logic/engine_libraries.h)
  if (!level_.logic.empty()) logic_->add(level_.logic, 0, "Level");
  for (const Actor& a : level_.actors)
    if (a.logic && !a.logic->empty()) logic_->add(*a.logic, static_cast<logic::ActorRef>(a.id), a.name);
}

void GameWorld::logic_fire(const char* type, ActorId actor, ActorId other, std::vector<std::pair<std::string, logic::Value>> extra, std::string key) {
  if (!logic_) return;
  // The actor's graph, or its nearest ancestor's (a welded collider of a door belongs to the door).
  ActorId target = actor;
  for (const Actor* a = level_.find(actor); a; a = level_.find(a->parent))
    if (logic_->find(static_cast<logic::ActorRef>(a->id))) { target = a->id; break; }
  logic::Event e;
  e.type = type;
  e.target = static_cast<logic::ActorRef>(target);
  e.level = true;
  e.key = std::move(key);
  e.args.push_back({"actor", act(actor)});
  e.args.push_back({"other", act(other)});
  for (auto& x : extra) e.args.push_back(std::move(x));
  if (!target) {  // events about nobody in particular (game over...) reach every graph
    e.target = 0;
    e.level = false;
  }
  logic_->fire(e);
}

void GameWorld::logic_inputs(Pawn& pawn, const PlayInput& in) {
  if (!logic_) { pawn.prev_input = in; return; }
  const PlayInput& was = pawn.prev_input;
  const struct { const char* name; bool now, before; } actions[] = {
      {"jump", in.jump, was.jump},     {"fire", in.fire, was.fire},       {"aim", in.aim, was.aim},
      {"reload", in.reload, was.reload}, {"crouch", in.crouch, was.crouch}, {"interact", in.interact, was.interact},
      {"boost", in.boost, was.boost}};
  for (const auto& a : actions)
    if (a.now != a.before) logic_fire("game.input", pawn.actor, 0, {{"pressed", logic::Value::boolean(a.now)}}, a.name);
  if (in.interact && !was.interact) interact(pawn);
  pawn.prev_input = in;
}

void GameWorld::interact(Pawn& pawn) {
  // What the player looks at within 3 m (first person: the aim ray; others: the facing ray at chest height).
  Mat4 m;
  if (!actor_matrix(pawn.actor, m)) return;
  const Actor* a = level_.find(pawn.actor);
  const bool soldier = a && a->controller && a->controller->mode == ControlMode::soldier;
  Vec3 from = m.translation(), dir;
  if (soldier) {
    from = pawn.mover.position() + Vec3{0, 0, a_eye_height(pawn.actor) * (1 - 0.4f * pawn.crouch)};
    const f32 y = pawn.yaw * kDegree, p = pawn.pitch * kDegree;
    dir = {kern::dm::cos(p) * kern::dm::cos(y), kern::dm::cos(p) * kern::dm::sin(y), -kern::dm::sin(p)};
  } else {
    dir = {kern::dm::cos(pawn.yaw * kDegree), kern::dm::sin(pawn.yaw * kDegree), 0};
  }
  ActorId hit = 0;
  Vec3 point, normal;
  if (line_trace(from, from + dir * 3.0f, pawn.actor, hit, point, normal) && hit) {
    logic_fire("game.interact", hit, pawn.actor, {{"point", logic::Value::vector(point)}});
    return;
  }
  // Nothing in the line: the nearest actor with a graph within 2 m.
  ActorId best = 0;
  f32 best_d = 2.0f;
  for (const auto& inst : logic_->instances()) {
    if (!inst->self() || inst->self() == pawn.actor) continue;
    Mat4 w;
    if (!actor_matrix(inst->self(), w)) continue;
    const f32 d = distance(w.translation(), m.translation());
    if (d < best_d) { best_d = d; best = inst->self(); }
  }
  if (best) logic_fire("game.interact", best, pawn.actor, {{"point", logic::Value::vector(m.translation())}});
}

void GameWorld::key_event(const std::string& code, bool down) {
  if (!logic_ || !running() || paused_) return;
  logic::Event e;
  e.type = "game.key";
  e.key = code;
  e.args.push_back({"pressed", logic::Value::boolean(down)});
  e.args.push_back({"actor", act(player_)});
  logic_->fire(e);
}

void GameWorld::call_logic_event(const std::string& name, ActorId target) {
  if (logic_) logic_->call(name, static_cast<logic::ActorRef>(target));
}

// ---------------------------------------------------------------------------
// replication

void GameWorld::take_replicated_changes(std::vector<PropertyChange>& props, std::vector<SpawnChange>& spawns) {
  props.insert(props.end(), prop_changes_.begin(), prop_changes_.end());
  spawns.insert(spawns.end(), spawn_changes_.begin(), spawn_changes_.end());
  prop_changes_.clear();
  spawn_changes_.clear();
}

void GameWorld::replicated_state(std::vector<PropertyChange>& props, std::vector<SpawnChange>& spawns) const {
  spawns = spawn_log_;
  props = prop_log_;
}

namespace {
/// The value at `path` in `root` (dots between keys, an index for a list or a vector's channel), null with the reason
/// where it stops - the keys there, or how long the list is.
json::Value* path_mut(json::Value& root, std::string_view path, std::string& why) {
  json::Value* v = &root;
  std::string at;
  while (!path.empty()) {
    const std::size_t dot = path.find('.');
    const std::string_view token = path.substr(0, dot);
    const std::string where = at.empty() ? std::string("the actor") : "\"" + at + "\"";
    if (v->is_array()) {
      bool digits = !token.empty() && token.size() < 10;
      for (char c : token) digits &= c >= '0' && c <= '9';
      const std::size_t i = digits ? static_cast<std::size_t>(std::atoi(std::string(token).c_str())) : 0;
      if (!digits || i >= v->size()) {
        why = where + " is a list of " + std::to_string(v->size()) + ": \"" + std::string(token) + "\" is no index of it (0.." +
              std::to_string(v->size() ? v->size() - 1 : 0) + ")";
        return nullptr;
      }
      v = &v->at(i);
    } else if (v->is_object()) {
      json::Value* next = v->find(token);
      if (!next) {
        std::string keys;
        for (const auto& [k, x] : v->members()) keys += (keys.empty() ? "" : ", ") + k;
        why = where + " has no \"" + std::string(token) + "\"" + (keys.empty() ? std::string(" (it has nothing)") : " - it has " + keys);
        return nullptr;
      }
      v = next;
    } else {
      why = where + " is a single value: it has no \"" + std::string(token) + "\"";
      return nullptr;
    }
    at += (at.empty() ? "" : ".") + std::string(token);
    path = dot == std::string_view::npos ? std::string_view() : path.substr(dot + 1);
  }
  return v;
}

const char* kind_name(const json::Value& v) {
  return v.is_number() ? "a number" : v.is_bool() ? "true or false" : v.is_string() ? "a text" : v.is_array() ? "a list" : v.is_object() ? "an object" : "nothing";
}

std::string actor_name(const Actor& a) { return a.name.empty() ? "actor " + std::to_string(a.id) : "actor \"" + a.name + "\""; }
}  // namespace

bool GameWorld::set_property_number(ActorId id, const std::string& path, f64 value) { return set_property(id, path, json::Value(value)); }
bool GameWorld::set_property_flag(ActorId id, const std::string& path, bool value) { return set_property(id, path, json::Value(value)); }
bool GameWorld::set_property_text(ActorId id, const std::string& path, const std::string& value) { return set_property(id, path, json::Value(value)); }

bool GameWorld::set_property(ActorId id, const std::string& path, const json::Value& value) {
  Actor* a = level_.find(id);
  if (!a) return fail("set property \"%s\": no actor %u in the world", path.c_str(), id);
  const std::string who = actor_name(*a);
  if (path.empty()) return fail("set property of %s: the path is empty (e.g. \"components.light.intensity\")", who.c_str());
  if (path == "id" || path == "parent" || path.rfind("components.logic", 0) == 0)
    return fail("set property \"%s\" of %s: it cannot be set while the world runs (an actor's id, parent and Blueprint stay)", path.c_str(), who.c_str());
  json::Value doc = actor_to_json(*a);
  std::string why;
  json::Value* target = path_mut(doc, path, why);
  if (!target) return fail("set property \"%s\" of %s: %s", path.c_str(), who.c_str(), why.c_str());
  json::Value v = value;
  auto wrong = [&](const char* wants) {
    return fail("set property \"%s\" of %s: it is %s, the value is %s", path.c_str(), who.c_str(), wants, kind_name(value));
  };
  if (target->is_number()) {
    if (v.is_string()) {   // (a text that is all a number: what the node "Set Property" gives)
      const std::string& t = v.as_string();
      char* end = nullptr;
      const f64 n = t.empty() ? 0 : std::strtod(t.c_str(), &end);
      if (t.empty() || *end) return wrong("a number");
      v = n;
    }
    if (!v.is_number()) return wrong("a number");
  } else if (target->is_bool()) {
    if (!v.is_bool()) return wrong("true or false");
  } else if (target->is_array() && target->size() == 3 && v.is_number()) {   // a number for a vector: all three
    json::Value arr = json::Value::array();
    for (int i = 0; i < 3; ++i) arr.push(v.as_number());
    v = std::move(arr);
  } else if (target->is_string() ? !v.is_string() : target->is_array() ? !v.is_array() : target->is_object() && !v.is_object()) {
    return wrong(kind_name(*target));
  }
  *target = std::move(v);
  Actor updated;
  std::string error;
  if (!actor_from_json(doc, updated, &error)) return fail("set property \"%s\" of %s: %s", path.c_str(), who.c_str(), error.c_str());
  updated.id = a->id;
  updated.parent = a->parent;
  updated.logic = std::move(a->logic);
  *a = std::move(updated);
  // Presentation follows (meshes, lights read the level every frame; bodies keep their shapes).
  if (path.rfind("transform", 0) == 0) {
    auto b = body_of_actor_.find(id);
    if (b == body_of_actor_.end() || !b->second->body.valid()) world_[id] = level_.world_matrix(id);
    else {  // a body (static colliders included) follows its actor's new transform
      const Mat4 wm = level_.world_matrix(id);
      const Transform t = matrix_transform(wm);
      physics_.set_pose(b->second->body, physics::Pose{t.position, euler_to_quat(t.rotation)});
      b->second->prev = physics_.pose(b->second->body);
    }
  }
  if (a->mesh) sync_actor_mesh(*a, world_[id]);
  if (mode_ != SimMode::replica) {
    PropertyChange c{id, path, *target};
    prop_changes_.push_back(c);
    bool replaced = false;
    for (PropertyChange& p : prop_log_)
      if (p.actor == id && p.path == path) { p.value = c.value; replaced = true; }
    if (!replaced && prop_log_.size() < 4096) prop_log_.push_back(std::move(c));
  }
  return true;
}

bool GameWorld::set_material_color(ActorId id, Vec3 color) {
  const Actor* a = level_.find(id);
  if (!a) return fail("set material color: no actor %u in the world", id);
  if (!a->mesh) return fail("set material color of %s: it has no mesh", actor_name(*a).c_str());
  return set_property(id, "components.mesh.material.color", json::Value(hex_color(color)));
}

ActorId GameWorld::spawn(std::string_view template_name, const Transform& at, ActorId id) {
  const std::string name(template_name);
  if (!running()) { fail("spawn \"%s\": the world does not run", name.c_str()); return 0; }
  const Actor* src = nullptr;
  std::string known;
  for (const Actor& a : templates_.actors) {
    if (!a.is_template) continue;
    if (a.name == template_name) { src = &a; break; }
    known += (known.empty() ? "" : ", ") + a.name;
  }
  if (!src) {
    if (known.empty()) fail("no template \"%s\" - the level has no templates (an actor with \"template\": true)", name.c_str());
    else fail("no template \"%s\" - the level's templates: %s", name.c_str(), known.c_str());
    return 0;
  }
  if (!finite(at.position) || !finite(at.rotation) || !finite(at.scale)) { fail("spawn \"%s\": the transform is not finite", name.c_str()); return 0; }
  for (int i = 0; i < 3; ++i)
    if (std::fabs(at.scale[i]) < 1e-4f) { fail("spawn \"%s\": the scale must not be zero", name.c_str()); return 0; }
  // The template's subtree in the level's order (root first).
  std::vector<const Actor*> subtree{src};
  for (std::size_t i = 0; i < subtree.size(); ++i)
    for (ActorId k : templates_.children(subtree[i]->id))
      if (const Actor* c = templates_.find(k)) subtree.push_back(c);
  // The next free ids: past every actor of the world and every template (a copy is never called like a template).
  ActorId root = id;
  if (!root) {
    root = level_.next_id;
    for (const Actor& a : level_.actors) root = max_of(root, a.id + 1);
    for (const Actor& a : templates_.actors) root = max_of(root, a.id + 1);
  }
  for (std::size_t i = 0; i < subtree.size(); ++i)
    if (level_.find(root + static_cast<ActorId>(i)) || templates_.find(root + static_cast<ActorId>(i))) {
      fail("spawn \"%s\": the ids %u..%u are taken", name.c_str(), root, root + static_cast<ActorId>(subtree.size() - 1));
      return 0;
    }
  std::unordered_map<ActorId, ActorId> remap;
  for (std::size_t i = 0; i < subtree.size(); ++i) remap[subtree[i]->id] = root + static_cast<ActorId>(i);
  std::vector<ActorId> created;
  for (const Actor* a : subtree) {
    Actor copy = *a;
    copy.id = remap[a->id];
    if (a == src) {
      copy.is_template = false;
      copy.parent = 0;
      copy.transform = at;
      copy.hidden = false;
    } else {
      copy.parent = remap[copy.parent];
    }
    for (JointComponent& j : copy.joints)
      if (remap.count(j.target)) j.target = remap[j.target];
    level_.add(std::move(copy));
    const ActorId made = remap[a->id];
    created.push_back(made);
    alive_[made] = true;
    order_.push_back(made);
  }
  level_.next_id = max_of(level_.next_id, root + static_cast<ActorId>(subtree.size()));
  for (ActorId aid : created) {
    world_[aid] = level_.world_matrix(aid);
    if (const Actor* a = level_.find(aid); a && a->mesh) sync_actor_mesh(*a, world_[aid]);
    if (const Actor* a = level_.find(aid); a && a->pickup) ++hud_.pickups_total;   // (collect all: what spawn makes counts)
  }
  Level part;
  part.world = level_.world;
  for (ActorId aid : created) part.actors.push_back(*level_.find(aid));
  for (BodyPlan& plan : plan_bodies(part)) {
    if (!plan.owner) continue;
    auto sb = std::make_unique<SimBody>();
    sb->body = create_body(plan);
    if (!sb->body.valid()) continue;
    if (const Actor* owner = level_.find(plan.owner)) sb->scale = matrix_transform(level_.world_matrix(owner->id)).scale;
    if (mode_ == SimMode::replica) physics_.set_body_type(sb->body, physics::BodyType::kinematic);
    actor_of_body_[sb->body.id] = plan.owner;
    sb->plan = std::move(plan);
    body_of_actor_[sb->plan.owner] = sb.get();
    bodies_.push_back(std::move(sb));
  }
  if (mode_ != SimMode::replica) {
    for (ActorId aid : created) if (const Actor* a = level_.find(aid)) create_joints(*a);
    for (ActorId aid : created) init_combat(aid);
    SpawnChange c{name, root, at};
    spawn_changes_.push_back(c);
    if (spawn_log_.size() < 4096) spawn_log_.push_back(c);
    if (logic_) {
      for (ActorId aid : created)
        if (const Actor* a = level_.find(aid); a && a->logic && !a->logic->empty()) logic_->add(*a->logic, static_cast<logic::ActorRef>(aid), a->name);
      logic_->begin_added();   // (the copy's Begin Play, now - also when a graph spawns it)
    }
  }
  return root;
}

void GameWorld::destroy_actor(ActorId id) {
  if (!level_.find(id)) return;
  set_actor_alive(id, false);
  for (std::size_t i = 0; i < respawns_.size();)
    if (respawns_[i].id == id) respawns_.erase(respawns_.begin() + static_cast<std::ptrdiff_t>(i));
    else ++i;
  if (auto h = health_.find(id); h != health_.end()) { h->second.dead = true; h->second.respawn_at = -1; }
  if (logic_) logic_->remove(static_cast<logic::ActorRef>(id));
}

// ---------------------------------------------------------------------------
// actor operations

bool GameWorld::teleport(ActorId id, Vec3 position, const Vec3* rotation_deg) {
  Actor* a = level_.find(id);
  if (!a || !finite(position)) return false;
  auto it = body_of_actor_.find(id);
  if (it != body_of_actor_.end() && it->second->body.valid()) {
    SimBody& b = *it->second;
    physics::Pose p = physics_.pose(b.body);
    p.position = position;
    if (rotation_deg) p.rotation = euler_to_quat(*rotation_deg);
    if (Pawn* pawn = pawn_of(id); pawn && pawn->use_mover) {
      pawn->mover.reset(position - Vec3{0, 0, pawn->mover_lift});
      if (rotation_deg) pawn->yaw = rotation_deg->z;
    }
    physics_.set_pose(b.body, p);
    b.prev = p;
    physics_.set_awake(b.body, true);
    // Static bodies are not in the snapshots (they never move by themselves): replicate the move.
    if (b.plan.motion == Motion::fixed && b.plan.owner == id && mode_ != SimMode::replica) {
      Mat4 parent;
      if (a->parent) parent = level_.world_matrix(a->parent);
      f64 inv[16];
      Vec3 local = position;
      if (a->parent && invert(parent, inv)) {
        Mat4 pi;
        for (int i = 0; i < 16; ++i) pi.m[i] = static_cast<f32>(inv[i]);
        local = pi.transform_point(position);
      }
      a->transform.position = local;
      if (rotation_deg) a->transform.rotation = *rotation_deg;
      auto record = [&](const char* path, Vec3 v) {
        json::Value arr = json::Value::array();
        arr.push(static_cast<f64>(v.x));
        arr.push(static_cast<f64>(v.y));
        arr.push(static_cast<f64>(v.z));
        PropertyChange c{id, path, arr};
        prop_changes_.push_back(c);
        for (PropertyChange& q : prop_log_) if (q.actor == id && q.path == path) { q.value = c.value; return; }
        if (prop_log_.size() < 4096) prop_log_.push_back(std::move(c));
      };
      record("transform.position", local);
      if (rotation_deg) record("transform.rotation", *rotation_deg);
    }
    return true;
  }
  // No body: the actor's transform (relative to its parent).
  Mat4 parent;
  if (a->parent) parent = world_.count(a->parent) ? world_[a->parent] : level_.world_matrix(a->parent);
  Vec3 local = position;
  f64 inv[16];
  if (a->parent && invert(parent, inv)) {
    Mat4 pi;
    for (int i = 0; i < 16; ++i) pi.m[i] = static_cast<f32>(inv[i]);
    local = pi.transform_point(position);
  }
  json::Value pos = json::Value::array();
  pos.push(static_cast<f64>(local.x));
  pos.push(static_cast<f64>(local.y));
  pos.push(static_cast<f64>(local.z));
  bool ok = set_property(id, "transform.position", pos);
  if (rotation_deg) {
    json::Value r = json::Value::array();
    r.push(static_cast<f64>(rotation_deg->x));
    r.push(static_cast<f64>(rotation_deg->y));
    r.push(static_cast<f64>(rotation_deg->z));
    ok = set_property(id, "transform.rotation", r) && ok;
  }
  world_[id] = level_.world_matrix(id);
  return ok;
}

Vec3 GameWorld::actor_velocity(ActorId id) const {
  if (const Pawn* p = const_cast<GameWorld*>(this)->pawn_of(id); p && p->use_mover) return p->mover.velocity();
  auto it = body_of_actor_.find(id);
  if (it == body_of_actor_.end() || !it->second->body.valid()) return {};
  return physics_.linear_velocity(it->second->body);
}

bool GameWorld::set_actor_velocity(ActorId id, Vec3 v) {
  if (Pawn* p = pawn_of(id); p && p->use_mover) { p->mover.set_velocity(v); return true; }
  auto it = body_of_actor_.find(id);
  if (it == body_of_actor_.end() || !it->second->body.valid()) return false;
  physics_.set_linear_velocity(it->second->body, v);
  physics_.set_awake(it->second->body, true);
  return true;
}

bool GameWorld::push_actor(ActorId id, Vec3 amount, bool impulse) {
  if (Pawn* p = pawn_of(id); p && p->use_mover) {
    // Characters: an impulse changes the velocity by impulse / mass (75 kg); a force for one step.
    const f32 mass = 75;
    const f32 h = 1.0f / clamp(level_.world.step_hz, 15.0f, 480.0f);
    p->mover.set_velocity(p->mover.velocity() + amount * ((impulse ? 1.0f : h) / mass));
    if (amount.z > 0) p->mover.jump(p->mover.velocity().z);
    return true;
  }
  const physics::Body b = actor_body(id);
  if (!b.valid() || physics_.body_type(b) != physics::BodyType::dynamic) return false;
  if (impulse) physics_.apply_linear_impulse_to_center(b, amount);
  else physics_.apply_force_to_center(b, amount);
  physics_.set_awake(b, true);
  return true;
}

bool GameWorld::set_simulate_physics(ActorId id, bool simulate) {
  auto it = body_of_actor_.find(id);
  if (it == body_of_actor_.end() || !it->second->body.valid()) return false;
  physics_.set_body_type(it->second->body, simulate ? physics::BodyType::dynamic : physics::BodyType::kinematic);
  physics_.set_awake(it->second->body, true);
  return true;
}

bool GameWorld::set_joint_motor(ActorId id, i32 joint, bool enable, f32 speed, f32 max_force) {
  for (auto& sj : joints_) {
    if (sj->actor != id || sj->broken || static_cast<i32>(sj->index) != joint) continue;
    const bool angular = sj->kind == JointKind::hinge || sj->kind == JointKind::wheel || sj->kind == JointKind::ball;
    physics_.set_joint_motor(sj->joint, enable, angular ? speed * kDegree : speed, max_force);
    return true;
  }
  return false;
}

ActorId GameWorld::find_actor(std::string_view name) const {
  for (const Actor& a : level_.actors) if (a.name == name && actor_alive(a.id)) return a.id;
  for (const Actor& a : level_.actors) if (a.name == name) return a.id;
  return 0;
}

void GameWorld::actors_with_tag(std::string_view tag, std::vector<ActorId>& out) const {
  out.clear();
  for (const Actor& a : level_.actors)
    if (actor_alive(a.id) && std::find(a.tags.begin(), a.tags.end(), tag) != a.tags.end()) out.push_back(a.id);
}

bool GameWorld::has_tag(ActorId id, std::string_view tag) const {
  const Actor* a = level_.find(id);
  return a && std::find(a->tags.begin(), a->tags.end(), tag) != a->tags.end();
}

bool GameWorld::line_trace(Vec3 from, Vec3 to, ActorId ignore, ActorId& actor, Vec3& point, Vec3& normal, physics::QueryFilter filter) {
  actor = 0;
  point = to;
  normal = {};
  if (!physics_.valid() || !finite(from) || !finite(to)) return false;
  const physics::RayHit hit = physics_.cast_ray(from, to - from, filter, actor_body(ignore));
  if (!hit.hit) return false;
  actor = static_cast<ActorId>(physics_.shape_user(hit.shape));
  if (!actor) actor = actor_for_body(hit.body);
  point = hit.point;
  normal = hit.normal;
  return true;
}

void GameWorld::add_score(f32 points) {
  if (hud_.status == GameStatus::playing || hud_.status == GameStatus::inactive) hud_.score += points;
  const WorldSettings& w = level_.world;
  if (w.goal == GameGoal::score && w.score_target > 0 && hud_.score >= w.score_target) finish(GameStatus::won, "Target score reached!");
}

void GameWorld::end_game(bool won, const std::string& message) {
  finish(won ? GameStatus::won : GameStatus::lost, message.empty() ? (won ? "You win!" : "Game over") : message);
}

void GameWorld::heal(ActorId id, f32 amount) {
  auto h = health_.find(id);
  if (h == health_.end() || h->second.dead) return;
  const Actor* a = level_.find(id);
  const f32 max = a && a->health ? a->health->max_health : 100;
  h->second.hp = min_of(max, h->second.hp + max_of(0.0f, amount));
}

void GameWorld::give_ammo(ActorId pawn, i32 rounds) {
  if (Pawn* p = pawn_of(pawn)) p->reserve = max_of(0, p->reserve + rounds);
}

}  // namespace kern::game
