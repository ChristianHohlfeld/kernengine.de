// Saving and loading (kern/docs/app.md 5., kern/app/storage.h): the platform's host of the slots and the Engine's
// save_* / load_*.
#include "kern/app/storage.h"

#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

#include "kern/app/app.h"

#if defined(__wasm__)
#define KERN_ENV(name) __attribute__((import_module("kern_env"), import_name(#name)))
extern "C" {
// (the page's localStorage, kern/web/kern.mjs): the length of the slot's text (-1: none, -2: cannot read), which
// storage_take then copies; storage_set / storage_remove: 1 done, 0 not (storage_error says why)
KERN_ENV(storage_get) kern::i32 kern_env_storage_get(const char* name, kern::u32 length);
KERN_ENV(storage_take) void kern_env_storage_take(char* out, kern::u32 capacity);
KERN_ENV(storage_set) kern::i32 kern_env_storage_set(const char* name, kern::u32 length, const char* text, kern::u32 text_length);
KERN_ENV(storage_remove) kern::i32 kern_env_storage_remove(const char* name, kern::u32 length);
KERN_ENV(storage_error) kern::u32 kern_env_storage_error(char* out, kern::u32 capacity);
}
#else
#include <sys/stat.h>
#endif

namespace kern {

bool storage_name_ok(std::string_view name) {
  if (name.empty() || name.size() > 64) return false;
  for (char c : name)
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.')) return false;
  return true;
}

namespace {

#if defined(__wasm__)
std::string page_error() {
  char b[256];
  const u32 n = kern_env_storage_error(b, sizeof b);
  return n ? std::string(b, n < sizeof b ? n : sizeof b) : std::string("the page's storage refused it");
}
class PageStorage final : public StorageHost {
 public:
  bool read(std::string_view slot, std::string& text, bool& found, std::string& why) override {
    const i32 n = kern_env_storage_get(slot.data(), static_cast<u32>(slot.size()));
    found = n >= 0;
    if (n == -2) { why = page_error(); return false; }
    text.assign(n > 0 ? static_cast<std::size_t>(n) : 0, '\0');
    if (n > 0) kern_env_storage_take(text.data(), static_cast<u32>(n));
    return true;
  }
  bool write(std::string_view slot, std::string_view text, std::string& why) override {
    if (kern_env_storage_set(slot.data(), static_cast<u32>(slot.size()), text.data(), static_cast<u32>(text.size()))) return true;
    why = page_error();
    return false;
  }
  bool remove(std::string_view slot, std::string& why) override {
    if (kern_env_storage_remove(slot.data(), static_cast<u32>(slot.size()))) return true;
    why = page_error();
    return false;
  }
};
#else
class FileStorage final : public StorageHost {
 public:
  explicit FileStorage(std::string dir) : dir_(std::move(dir)) {}
  std::string path(std::string_view slot) const { return dir_ + "/" + std::string(slot) + ".kern-save"; }
  bool read(std::string_view slot, std::string& text, bool& found, std::string& why) override {
    FILE* f = std::fopen(path(slot).c_str(), "rb");
    found = f != nullptr;
    if (!f) {
      if (errno == ENOENT) return true;
      why = path(slot) + ": " + std::strerror(errno);
      return false;
    }
    text.clear();
    char b[4096];
    for (std::size_t n; (n = std::fread(b, 1, sizeof b, f)) > 0;) text.append(b, n);
    std::fclose(f);
    return true;
  }
  bool write(std::string_view slot, std::string_view text, std::string& why) override {
    ::mkdir(dir_.c_str(), 0755);   // (there already: nothing)
    const std::string p = path(slot), tmp = p + ".new";
    FILE* f = std::fopen(tmp.c_str(), "wb");
    if (!f) { why = tmp + ": " + std::strerror(errno); return false; }
    const bool ok = std::fwrite(text.data(), 1, text.size(), f) == text.size();
    if (std::fclose(f) != 0 || !ok || std::rename(tmp.c_str(), p.c_str()) != 0) {
      why = p + ": " + std::strerror(errno);
      std::remove(tmp.c_str());
      return false;
    }
    return true;
  }
  bool remove(std::string_view slot, std::string& why) override {
    if (std::remove(path(slot).c_str()) == 0 || errno == ENOENT) return true;
    why = path(slot) + ": " + std::strerror(errno);
    return false;
  }
 private:
  std::string dir_;
};
#endif

json::Value typed(const char* type, json::Value v) {
  json::Value o = json::Value::object();
  o.set(type, std::move(v));
  return o;
}

}  // namespace

StorageHost& platform_storage(std::string dir) {
#if defined(__wasm__)
  (void)dir;
  static PageStorage host;
  return host;
#else
  // (one host a folder, kept to the end of the process)
  static std::vector<std::unique_ptr<FileStorage>> hosts;
  for (const std::unique_ptr<FileStorage>& h : hosts)
    if (h->path("") == FileStorage(dir).path("")) return *h;
  hosts.push_back(std::make_unique<FileStorage>(std::move(dir)));
  return *hosts.back();
#endif
}

// ---- the Engine's slots ----

void Engine::set_storage(StorageHost& host) {
  storage_ = &host;
  slots_.clear();
}

Engine::Slot* Engine::slot(const char* name, const char* key, bool for_write) {
  const std::string n = name ? name : "";
  if (!storage_name_ok(n)) { fail("save slot \"%s\": a slot's name is 1 to 64 of A-Z a-z 0-9 _ - .", n.c_str()); return nullptr; }
  if (key && !storage_name_ok(key)) { fail("save slot %s: the key \"%s\" - a key is 1 to 64 of A-Z a-z 0-9 _ - .", n.c_str(), key); return nullptr; }
  if (!storage_) storage_ = &platform_storage(arg("saves", "kern-saves"));
  for (Slot& s : slots_)
    if (s.name == n) {
      if (!s.broken.empty()) { fail("save slot %s: %s - it is not %s until it is erased", n.c_str(), s.broken.c_str(), for_write ? "written over" : "read"); return nullptr; }
      return &s;
    }
  std::string text, why;
  bool found = false;
  if (!storage_->read(n, text, found, why)) { fail("save slot %s: the host cannot read it: %s", n.c_str(), why.c_str()); return nullptr; }
  Slot s;
  s.name = n;
  s.values = json::Value::object();
  if (found) {
    json::Value doc;
    std::string perr;
    if (!json::parse(text, doc, &perr)) s.broken = "its text is no JSON (" + perr + ")";
    else if (doc["format"].as_string() != "kern-save") s.broken = "it is no kern-save (format must be \"kern-save\")";
    else if (doc["version"].as_number(0) > 1) s.broken = "it was written by a newer Kern";
    else if (!doc["values"].is_object()) s.broken = "its values are no object";
    else s.values = doc["values"];
  }
  slots_.push_back(std::move(s));
  Slot& made = slots_.back();
  if (!made.broken.empty()) { fail("save slot %s: %s - it is not %s until it is erased", n.c_str(), made.broken.c_str(), for_write ? "written over" : "read"); return nullptr; }
  return &made;
}

bool Engine::store(Slot& s, const char* key, json::Value value) {
  const json::Value before = s.values;
  s.values.set(key, std::move(value));
  json::Value doc = json::Value::object();
  doc.set("format", "kern-save");
  doc.set("version", 1);
  doc.set("values", s.values);
  std::string why;
  if (storage_->write(s.name, json::stringify(doc, 1), why)) return true;
  s.values = before;   // (what the host does not hold, the slot does not hold either)
  return fail("save slot %s: the host cannot write it: %s", s.name.c_str(), why.c_str());
}

bool Engine::save_int(const char* slot_name, const char* key, i64 value) {
  Slot* s = slot(slot_name, key, true);
  return s && store(*s, key, typed("int", std::to_string(value)));
}
bool Engine::save_float(const char* slot_name, const char* key, f64 value) {
  Slot* s = slot(slot_name, key, true);
  if (!s) return false;
  if (!std::isfinite(value)) return fail("save slot %s: %s is not a number that can be kept (%g)", slot_name, key, value);
  return store(*s, key, typed("float", value));
}
bool Engine::save_bool(const char* slot_name, const char* key, bool value) {
  Slot* s = slot(slot_name, key, true);
  return s && store(*s, key, typed("bool", value));
}
bool Engine::save_text(const char* slot_name, const char* key, const char* value) {
  Slot* s = slot(slot_name, key, true);
  return s && store(*s, key, typed("text", value ? value : ""));
}

const json::Value* Engine::stored(const char* slot_name, const char* key, const char* type) {
  Slot* s = slot(slot_name, key, false);
  if (!s) return nullptr;
  const json::Value* v = s->values.find(key);
  if (!v) return nullptr;   // (none: the fallback, no error)
  if (const json::Value* x = v->find(type)) return x;
  const char* has = v->find("int") ? "an int" : v->find("float") ? "a float" : v->find("bool") ? "a bool" : v->find("text") ? "a text" : "no value";
  fail("save slot %s: %s holds %s, not %s %s", slot_name, key, has, std::strcmp(type, "int") == 0 ? "an" : "a", type);
  return nullptr;
}

i64 Engine::load_int(const char* slot_name, const char* key, i64 fallback) {
  const json::Value* x = stored(slot_name, key, "int");
  if (!x) return fallback;
  const std::string& t = x->as_string();
  char* end = nullptr;
  errno = 0;
  const long long v = std::strtoll(t.c_str(), &end, 10);
  if (t.empty() || *end || errno) { fail("save slot %s: %s holds no whole number (\"%s\")", slot_name, key, t.c_str()); return fallback; }
  return static_cast<i64>(v);
}
f64 Engine::load_float(const char* slot_name, const char* key, f64 fallback) {
  const json::Value* x = stored(slot_name, key, "float");
  return x && x->is_number() ? x->as_number() : fallback;
}
bool Engine::load_bool(const char* slot_name, const char* key, bool fallback) {
  const json::Value* x = stored(slot_name, key, "bool");
  return x && x->is_bool() ? x->as_bool() : fallback;
}
const char* Engine::load_text(const char* slot_name, const char* key, const char* fallback) {
  const json::Value* x = stored(slot_name, key, "text");
  loaded_text_ = x && x->is_string() ? x->as_string() : std::string(fallback ? fallback : "");
  return loaded_text_.c_str();
}
bool Engine::saved(const char* slot_name, const char* key) {
  Slot* s = slot(slot_name, key, false);
  return s && s->values.find(key);
}
bool Engine::erase_saved(const char* slot_name) {
  const std::string n = slot_name ? slot_name : "";
  if (!storage_name_ok(n)) return fail("save slot \"%s\": a slot's name is 1 to 64 of A-Z a-z 0-9 _ - .", n.c_str());
  if (!storage_) storage_ = &platform_storage(arg("saves", "kern-saves"));
  std::string why;
  if (!storage_->remove(n, why)) return fail("save slot %s: the host cannot remove it: %s", n.c_str(), why.c_str());
  for (std::size_t i = 0; i < slots_.size(); ++i)
    if (slots_[i].name == n) { slots_.erase(slots_.begin() + static_cast<std::ptrdiff_t>(i)); break; }
  return true;
}

}  // namespace kern
