// Drawing for the M3 world (see world_art.h).
#include "world_art.h"

#include "kern/geo/primitives.h"

namespace sc {

namespace prim = kern::primitives;
using kern::Mat4;

bool WorldArt::init(kern::Renderer& r) {
  r_ = &r;
  parts_.clear();
  parts_.push_back({});  // id 0 = none
  kern::Material m;
  m.roughness = 0.85f;
  cube = part(prim::box({1, 1, 1}), m, true);
  sphere = part(prim::sphere(1, 12, 8), m, true);
  kern::MeshData cyl;
  prim::append(cyl, prim::cylinder(1, 1, 12), Mat4::translate({0, 0, 0.5f}));
  cylinder = part(cyl, m, true);
  kern::Material lit;
  lit.unlit = true;
  lit.emissive = {2.2f, 2.2f, 2.2f};
  lit.cast_shadow = false;
  glow = part(prim::sphere(1, 10, 6), lit, true);
  return cube && sphere && cylinder && glow;
}

WorldArt::PartId WorldArt::part(const kern::MeshData& mesh, const kern::Material& m, bool tinted) {
  Part p;
  p.id = r_->allocate_id();
  p.tinted = tinted;
  kern::MeshInput in = mesh.as_input(kern::kMaxExtensionAttributes);
  in.material = m;
  in.visible = false;
  static const Mat4 parked = Mat4::translate({0, 0, -1000});
  in.transforms = kern::Span<const f32>(parked.m, 16);
  if (!r_->upsert(p.id, in)) { r_->release_id(p.id); return 0; }
  parts_.push_back(std::move(p));
  return static_cast<PartId>(parts_.size() - 1);
}

void WorldArt::begin() {
  for (Part& p : parts_) { p.transforms.clear(); p.tints.clear(); }
}

void WorldArt::put(PartId id, const Mat4& m, Vec3 tint) {
  if (id == 0 || id >= parts_.size()) return;
  Part& p = parts_[id];
  p.transforms.insert(p.transforms.end(), m.m, m.m + 16);
  if (p.tinted) p.tints.insert(p.tints.end(), {tint.x, tint.y, tint.z, 1});
}

void WorldArt::end() {
  for (Part& p : parts_) {
    if (!p.id) continue;
    if (p.transforms.empty()) { r_->set_visible(p.id, false); continue; }
    r_->update_instances(p.id, kern::Span<const f32>(p.transforms.data(), p.transforms.size()), kern::Span<const f32>(p.tints.data(), p.tints.size()));
    r_->set_visible(p.id, true);
  }
}

void WorldArt::set_mesh(u32& slot, const kern::MeshData& mesh, const kern::Material& m) {
  if (mesh.vertex_count() == 0) {
    if (slot) r_->remove(slot);
    slot = 0;
    return;
  }
  kern::MeshInput in = mesh.as_input();
  in.material = m;
  if (!slot) slot = r_->allocate_id();
  if (!r_->upsert(slot, in)) { r_->release_id(slot); slot = 0; }
}

}  // namespace sc
