// MVT decoder / minimal writer (see mvt.h). Spec: github.com/mapbox/vector-tile-spec (2.1).
#include "mvt.h"

#include <cstring>

#include "kern/core/base.h"

namespace sc::mvt {

namespace {

struct Reader {
  const u8* p;
  const u8* end;
  bool ok = true;
  bool more() const { return ok && p < end; }
  u64 varint() {
    u64 v = 0;
    int shift = 0;
    while (p < end && shift < 64) {
      const u8 b = *p++;
      v |= static_cast<u64>(b & 0x7f) << shift;
      if (!(b & 0x80)) return v;
      shift += 7;
    }
    ok = false;
    return 0;
  }
  Reader sub() {
    const u64 n = varint();
    if (!ok || n > static_cast<u64>(end - p)) { ok = false; return {end, end, false}; }
    Reader r{p, p + n};
    p += n;
    return r;
  }
  std::string_view bytes() {
    const u64 n = varint();
    if (!ok || n > static_cast<u64>(end - p)) { ok = false; return {}; }
    std::string_view s(reinterpret_cast<const char*>(p), n);
    p += n;
    return s;
  }
  void skip(u32 wire) {
    switch (wire) {
      case 0: varint(); break;
      case 1: p += 8; break;
      case 2: { const u64 n = varint(); if (n > static_cast<u64>(end - p)) ok = false; else p += n; break; }
      case 5: p += 4; break;
      default: ok = false;
    }
    if (p > end) ok = false;
  }
  f64 fixed64() {
    if (end - p < 8) { ok = false; return 0; }
    f64 d;
    std::memcpy(&d, p, 8);
    p += 8;
    return d;
  }
  f32 fixed32() {
    if (end - p < 4) { ok = false; return 0; }
    f32 f;
    std::memcpy(&f, p, 4);
    p += 4;
    return f;
  }
};

i64 zigzag(u64 v) { return static_cast<i64>(v >> 1) ^ -static_cast<i64>(v & 1); }

Value read_value(Reader r) {
  Value v;
  while (r.more()) {
    const u64 key = r.varint();
    const u32 field = static_cast<u32>(key >> 3), wire = static_cast<u32>(key & 7);
    switch (field) {
      case 1: v.kind = Value::Kind::string; v.s = std::string(r.bytes()); break;
      case 2: v.kind = Value::Kind::number; v.n = r.fixed32(); break;
      case 3: v.kind = Value::Kind::number; v.n = r.fixed64(); break;
      case 4: v.kind = Value::Kind::number; v.n = static_cast<f64>(static_cast<i64>(r.varint())); break;
      case 5: v.kind = Value::Kind::number; v.n = static_cast<f64>(r.varint()); break;
      case 6: v.kind = Value::Kind::number; v.n = static_cast<f64>(zigzag(r.varint())); break;
      case 7: v.kind = Value::Kind::boolean; v.b = r.varint() != 0; break;
      default: r.skip(wire);
    }
  }
  return v;
}

void read_geometry(Reader r, GeomType type, std::vector<std::vector<Vec2>>& parts) {
  i64 x = 0, y = 0;
  std::vector<Vec2>* cur = nullptr;
  while (r.more()) {
    const u64 c = r.varint();
    const u32 id = static_cast<u32>(c & 7), count = static_cast<u32>(c >> 3);
    if (id == 1 || id == 2) {
      for (u32 i = 0; i < count && r.more(); ++i) {
        x += zigzag(r.varint());
        y += zigzag(r.varint());
        if (id == 1 || !cur) { parts.emplace_back(); cur = &parts.back(); }
        cur->push_back({static_cast<f32>(x), static_cast<f32>(y)});
      }
    } else if (id == 7) {
      // ClosePath: rings are implicitly closed (no repeated point).
    } else {
      return;
    }
  }
  (void)type;
}

}  // namespace

const Value* Layer::tag(const Feature& f, std::string_view key) const {
  for (const auto& [k, v] : f.tags)
    if (k < keys.size() && v < values.size() && keys[k] == key) return &values[v];
  return nullptr;
}

std::string_view Layer::text(const Feature& f, std::string_view key) const {
  const Value* v = tag(f, key);
  return v && v->kind == Value::Kind::string ? std::string_view(v->s) : std::string_view();
}

f64 Layer::number(const Feature& f, std::string_view key, f64 fallback) const {
  const Value* v = tag(f, key);
  return v && v->kind == Value::Kind::number ? v->n : fallback;
}

const Layer* Tile::layer(std::string_view name) const {
  for (const Layer& l : layers)
    if (l.name == name) return &l;
  return nullptr;
}

f64 ring_area(const std::vector<Vec2>& ring) {
  f64 a = 0;
  const std::size_t n = ring.size();
  for (std::size_t i = 0, j = n ? n - 1 : 0; i < n; j = i++) a += static_cast<f64>(ring[j].x) * ring[i].y - static_cast<f64>(ring[i].x) * ring[j].y;
  return a * 0.5;
}

namespace {

bool wanted_layer(const std::string& name, const std::vector<std::string>& wanted) {
  if (wanted.empty()) return true;
  for (const std::string& w : wanted)
    if (w == name) return true;
  return false;
}

/// Layer header (name, keys, values, extent) and the spans of its features.
bool open_layer(Reader lr, Layer& layer, std::vector<std::pair<const u8*, const u8*>>& features) {
  while (lr.more()) {
    const u64 k = lr.varint();
    const u32 field = static_cast<u32>(k >> 3), wire = static_cast<u32>(k & 7);
    switch (field) {
      case 1: layer.name = std::string(lr.bytes()); break;
      case 2: { Reader f = lr.sub(); features.push_back({f.p, f.end}); break; }
      case 3: layer.keys.emplace_back(lr.bytes()); break;
      case 4: layer.values.push_back(read_value(lr.sub())); break;
      case 5: layer.extent = static_cast<u32>(lr.varint()); break;
      default: lr.skip(wire);
    }
  }
  return lr.ok;
}

bool read_feature(Reader fr, Feature& f) {
  Reader geometry{nullptr, nullptr};
  bool has_geometry = false;
  while (fr.more()) {
    const u64 k = fr.varint();
    const u32 field = static_cast<u32>(k >> 3), wire = static_cast<u32>(k & 7);
    if (field == 1 && wire == 0) f.id = fr.varint();
    else if (field == 2 && wire == 2) {
      Reader t = fr.sub();
      while (t.more()) {
        const u32 a = static_cast<u32>(t.varint());
        const u32 b = static_cast<u32>(t.varint());
        f.tags.push_back({a, b});
      }
    } else if (field == 3 && wire == 0) f.type = static_cast<GeomType>(fr.varint());
    else if (field == 4 && wire == 2) { geometry = fr.sub(); has_geometry = true; }
    else fr.skip(wire);
  }
  if (!fr.ok) return false;
  if (has_geometry) read_geometry(geometry, f.type, f.parts);
  return true;
}

}  // namespace

void Decoder::begin(const u8* bytes, std::size_t length, const std::vector<std::string>& wanted) {
  layers_.clear();
  features_.clear();
  tile_.layers.clear();
  layer_ = feature_ = 0;
  layer_open_ = false;
  done_ = false;
  failed_ = false;
  Reader tile{bytes, bytes + length};
  while (tile.more()) {
    const u64 key = tile.varint();
    if ((key >> 3) != 3 || (key & 7) != 2) { tile.skip(static_cast<u32>(key & 7)); continue; }
    Reader lr = tile.sub();
    // Peek at the name to skip unwanted layers without decoding them.
    std::string name;
    Reader peek = lr;
    while (peek.more()) {
      const u64 k = peek.varint();
      if ((k >> 3) == 1 && (k & 7) == 2) { name = std::string(peek.bytes()); break; }
      peek.skip(static_cast<u32>(k & 7));
    }
    if (wanted_layer(name, wanted)) layers_.push_back({lr.p, lr.end});
  }
  if (!tile.ok) { failed_ = true; done_ = true; }
}

bool Decoder::step(f64 budget_ms) {
  if (done_) return true;
  const f64 start = kern::now_ms();
  u32 work = 0;
  while (layer_ < layers_.size()) {
    if (!layer_open_) {
      Layer layer;
      std::vector<std::pair<const u8*, const u8*>> spans;
      if (!open_layer(Reader{layers_[layer_].p, layers_[layer_].end}, layer, spans)) { failed_ = done_ = true; return true; }
      features_.clear();
      for (const auto& [a, b] : spans) features_.push_back({a, b});
      layer.features.reserve(features_.size());
      tile_.layers.push_back(std::move(layer));
      feature_ = 0;
      layer_open_ = true;
    }
    Layer& layer = tile_.layers.back();
    while (feature_ < features_.size()) {
      Feature f;
      if (!read_feature(Reader{features_[feature_].p, features_[feature_].end}, f)) { failed_ = done_ = true; return true; }
      layer.features.push_back(std::move(f));
      ++feature_;
      if ((++work & 63) == 0 && budget_ms > 0 && kern::now_ms() - start > budget_ms) return false;
    }
    layer_open_ = false;
    ++layer_;
  }
  done_ = true;
  return true;
}

bool decode(const u8* bytes, std::size_t length, Tile& out, const std::vector<std::string>& wanted) {
  Decoder d;
  d.begin(bytes, length, wanted);
  d.step(0);
  out = std::move(d.tile());
  return !d.failed();
}

// ---------------------------------------------------------------------------- writer

namespace {
void put_varint(std::vector<u8>& o, u64 v) {
  while (v >= 0x80) { o.push_back(static_cast<u8>(v | 0x80)); v >>= 7; }
  o.push_back(static_cast<u8>(v));
}
void put_key(std::vector<u8>& o, u32 field, u32 wire) { put_varint(o, (static_cast<u64>(field) << 3) | wire); }
void put_bytes(std::vector<u8>& o, u32 field, const void* data, std::size_t n) {
  put_key(o, field, 2);
  put_varint(o, n);
  const u8* p = static_cast<const u8*>(data);
  o.insert(o.end(), p, p + n);
}
u64 zz(i64 v) { return (static_cast<u64>(v) << 1) ^ static_cast<u64>(v >> 63); }
}  // namespace

void Writer::begin_layer(const std::string& name, u32 extent) {
  layer_ = PendingLayer{};
  layer_.name = name;
  layer_.extent = extent;
}

void Writer::feature(GeomType type, const std::vector<std::vector<Vec2>>& parts, const std::vector<std::pair<std::string, Value>>& tags, u64 id) {
  std::vector<u8> f;
  if (id) { put_key(f, 1, 0); put_varint(f, id); }
  std::vector<u8> t;
  for (const auto& [k, v] : tags) {
    u32 ki = 0, vi = 0;
    for (; ki < layer_.keys.size() && layer_.keys[ki] != k; ++ki) {}
    if (ki == layer_.keys.size()) layer_.keys.push_back(k);
    for (; vi < layer_.values.size(); ++vi) {
      const Value& o = layer_.values[vi];
      if (o.kind == v.kind && o.s == v.s && o.n == v.n && o.b == v.b) break;
    }
    if (vi == layer_.values.size()) layer_.values.push_back(v);
    put_varint(t, ki);
    put_varint(t, vi);
  }
  put_bytes(f, 2, t.data(), t.size());
  put_key(f, 3, 0);
  put_varint(f, static_cast<u64>(type));
  std::vector<u8> g;
  i64 x = 0, y = 0;
  for (const auto& part : parts) {
    if (part.empty()) continue;
    put_varint(g, (1u << 3) | 1);
    const i64 px = static_cast<i64>(std::lround(part[0].x)), py = static_cast<i64>(std::lround(part[0].y));
    put_varint(g, zz(px - x));
    put_varint(g, zz(py - y));
    x = px; y = py;
    if (part.size() > 1) {
      put_varint(g, (static_cast<u64>(part.size() - 1) << 3) | 2);
      for (std::size_t i = 1; i < part.size(); ++i) {
        const i64 qx = static_cast<i64>(std::lround(part[i].x)), qy = static_cast<i64>(std::lround(part[i].y));
        put_varint(g, zz(qx - x));
        put_varint(g, zz(qy - y));
        x = qx; y = qy;
      }
    }
    if (type == GeomType::polygon) put_varint(g, (1u << 3) | 7);
  }
  put_bytes(f, 4, g.data(), g.size());
  layer_.features.push_back(std::move(f));
}

void Writer::end_layer() {
  std::vector<u8> l;
  put_key(l, 15, 0);
  put_varint(l, 2);
  put_bytes(l, 1, layer_.name.data(), layer_.name.size());
  for (const auto& f : layer_.features) put_bytes(l, 2, f.data(), f.size());
  for (const auto& k : layer_.keys) put_bytes(l, 3, k.data(), k.size());
  for (const Value& v : layer_.values) {
    std::vector<u8> e;
    switch (v.kind) {
      case Value::Kind::string: put_bytes(e, 1, v.s.data(), v.s.size()); break;
      case Value::Kind::number: { put_key(e, 3, 1); const u8* p = reinterpret_cast<const u8*>(&v.n); e.insert(e.end(), p, p + 8); break; }
      case Value::Kind::boolean: put_key(e, 7, 0); put_varint(e, v.b ? 1 : 0); break;
      default: break;
    }
    put_bytes(l, 4, e.data(), e.size());
  }
  put_key(l, 5, 0);
  put_varint(l, layer_.extent);
  put_bytes(out_, 3, l.data(), l.size());
}

}  // namespace sc::mvt
