// Critical damage clocks and the rule ballot (critical_damage_logic.mjs, rules_vote_engine.mjs).
#include "combat_detail.h"

#include <algorithm>

namespace sc::detail {

bool CriticalClock::update(Critical k, f32 hp, f32 max_hp, f32 now) {
  const bool hit = last_hp >= 0 && hp < last_hp - 1e-3f;
  last_hp = hp;
  if (!armed()) {
    if (hp > 0 && is_critical(k, hp, max_hp)) at = detonation_at(k, now);
    return false;
  }
  if (!is_critical(k, hp, max_hp) && hp > 0) { at = -1; return false; }  // repaired / replaced
  if (hit) at = accelerate(k, at, now);
  if (now >= at) { at = -1; last_hp = -1; return true; }
  return false;
}

namespace {
void sort_unique(std::vector<u32>& v) {
  std::sort(v.begin(), v.end());
  v.erase(std::unique(v.begin(), v.end()), v.end());
}
bool has(const std::vector<u32>& v, u32 x) { return std::find(v.begin(), v.end(), x) != v.end(); }
}  // namespace

Vote create_ballot(const std::string& id, const std::string& key, bool value, u32 host, std::vector<u32> members, f64 now) {
  Vote b;
  sort_unique(members);
  if (id.empty() || key.empty() || !has(members, host) || members.size() < 2) return b;  // "invalid ballot"
  b.id = id;
  b.key = key;
  b.value = value;
  b.host = host;
  b.members = members;
  b.yes = {host};
  b.expires = now + kBallotSeconds;
  b.status = Ballot::pending;
  return b;
}

Ballot cast_vote(Vote& b, u32 peer, bool approve, std::vector<u32> current, f64 now) {
  if (b.status != Ballot::pending) return b.status;
  sort_unique(current);
  if (now >= b.expires || current != b.members) return b.status = Ballot::cancelled;
  if (!has(b.members, peer) || has(b.yes, peer)) return Ballot::ignored;
  if (!approve) return b.status = Ballot::rejected;
  b.yes.push_back(peer);
  bool all = true;
  for (u32 m : b.members) all = all && has(b.yes, m);
  if (all) b.status = Ballot::approved;
  return b.status;
}

const char* ballot_name(Ballot b) {
  switch (b) {
    case Ballot::pending: return "pending";
    case Ballot::approved: return "approved";
    case Ballot::rejected: return "rejected";
    case Ballot::cancelled: return "cancelled";
    case Ballot::ignored: return "ignored";
    default: return "invalid";
  }
}

}  // namespace sc::detail
