// The game's calls into the Blueprint of the actor "CombatDetail" (see combat_detail.h): no rule
// lives here.
#include "combat_detail.h"

#include "blueprint_call.h"

namespace sc::detail {

namespace {
using kern::logic::Value;
using rules::of_enum;
rules::Blueprint g_rules;
kern::logic::Instance*& g_blueprint = g_rules.running;

bool call(const char* function, const std::vector<Value>& args, std::vector<Value>& results) { return g_rules.call(function, args, results); }
f64 number(const char* function, const std::vector<Value>& args) { return g_rules.number(function, args); }
Value numbers(const std::vector<u32>& v) {
  std::vector<i64> items(v.begin(), v.end());
  return Value::of(std::move(items));
}
std::vector<u32> numbers(const Value* v) {
  std::vector<u32> r;
  for (std::size_t k = 0; v && k < v->size(); ++k) r.push_back(static_cast<u32>(v->item(k).i));
  return r;
}
// The Blueprint's struct Vote, field by field in its order.
Value to_value(const Vote& b) {
  return Value::structure({Value::text(b.id), Value::text(b.key), Value::boolean(b.value), Value::integer(b.host), numbers(b.members), numbers(b.yes), rules::number(b.expires),
                           of_enum(b.status)});
}
Vote to_vote(const Value& v) {
  Vote b;
  if (v.fields() < 8) return b;
  b.id = v.field(0)->as_text();
  b.key = v.field(1)->as_text();
  b.value = v.field(2)->b;
  b.host = static_cast<u32>(v.field(3)->i);
  b.members = numbers(v.field(4));
  b.yes = numbers(v.field(5));
  b.expires = v.field(6)->as_number();
  b.status = static_cast<Ballot>(v.field(7)->i);
  return b;
}
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_blueprint = blueprint; }
kern::logic::Instance* blueprint() { return g_blueprint; }

bool is_critical(Critical k, f32 hp, f32 max_hp) {
  std::vector<Value> r;
  return call("IsCritical", {of_enum(k), rules::number(hp), rules::number(max_hp)}, r) && !r.empty() && r[0].b;
}
f32 detonation_at(Critical k, f32 now) { return static_cast<f32>(number("DetonationAt", {of_enum(k), rules::number(now)})); }
f32 accelerate(Critical k, f32 current_at, f32 now) { return static_cast<f32>(number("Accelerate", {of_enum(k), rules::number(current_at), rules::number(now)})); }

bool CriticalClock::update(Critical k, f32 hp, f32 max_hp, f32 now) {
  std::vector<Value> r;
  if (!call("ClockUpdate", {of_enum(k), rules::number(hp), rules::number(max_hp), rules::number(now), rules::number(at), rules::number(last_hp)}, r) || r.size() < 3) return false;
  at = static_cast<f32>(r[1].as_number());
  last_hp = static_cast<f32>(r[2].as_number());
  return r[0].b;
}

f32 momentum_ns(Round r) { return static_cast<f32>(number("MomentumNs", {of_enum(r)})); }
Vec3 bullet_impulse(Vec3 direction, Round round) {
  std::vector<Value> r;
  return call("BulletImpulse", {Value::vector(direction), of_enum(round)}, r) && !r.empty() ? r[0].as_vector() : Vec3{};
}
f32 projectile_gravity() { return g_blueprint ? static_cast<f32>(g_blueprint->get("ProjectileGravity").as_number()) : 0; }

Vote create_ballot(const std::string& id, const std::string& key, bool value, u32 host, std::vector<u32> members, f64 now) {
  std::vector<Value> r;
  if (!call("CreateBallot", {Value::text(id), Value::text(key), Value::boolean(value), Value::integer(host), numbers(members), rules::number(now)}, r) || r.empty()) return {};
  return to_vote(r[0]);
}

Ballot cast_vote(Vote& b, u32 peer, bool approve, std::vector<u32> current, f64 now) {
  std::vector<Value> r;
  if (!call("CastVote", {to_value(b), Value::integer(peer), Value::boolean(approve), numbers(current), rules::number(now)}, r) || r.size() < 2) return b.status;
  b = to_vote(r[0]);
  return static_cast<Ballot>(r[1].i);
}

std::string ballot_name(Ballot b) {
  std::vector<Value> r;
  return call("BallotName", {of_enum(b)}, r) && !r.empty() ? r[0].as_text() : std::string();
}

}  // namespace sc::detail
