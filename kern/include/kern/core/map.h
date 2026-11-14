// Kern: maps and sets whose order is the order their keys came in.
//
// kern::Map<K, V> and kern::Set<K> hold their entries in the order their keys were first put in
// (as a JavaScript Map or Set does): going through one gives that order on every machine, in every
// build. A key is found in constant time (an index by a hash that is the same everywhere). Taking
// an entry out keeps the order of the others; an entry put in again goes to the end.
//
// The same containers are a Blueprint's Map and Set (kern/logic/nodes.h): what a program does with
// one and what its Blueprint does are the same steps on the same data, in every tier.
#pragma once

#include <cstddef>
#include <initializer_list>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "kern/core/base.h"

namespace kern {

/// The hash of a key - the same on every machine and in every build (where an entry stands does
/// not depend on it: only how fast it is found).
template <class K, class = void> struct KeyHash;
template <class K> struct KeyHash<K, std::enable_if_t<std::is_integral_v<K> || std::is_enum_v<K>>> {
  u64 operator()(K key) const {
    u64 x;
    if constexpr (std::is_enum_v<K>) x = static_cast<u64>(static_cast<std::underlying_type_t<K>>(key));
    else x = static_cast<u64>(key);
    x += 0x9e3779b97f4a7c15ull;  // (splitmix64)
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
    return x ^ (x >> 31);
  }
};
template <class K> struct KeyHash<K, std::enable_if_t<std::is_convertible_v<const K&, std::string_view> && !std::is_integral_v<K>>> {
  u64 operator()(const K& key) const {
    u64 h = 0xcbf29ce484222325ull;  // (FNV-1a)
    for (const char c : std::string_view(key)) h = (h ^ static_cast<u8>(c)) * 0x100000001b3ull;
    return h;
  }
};
/// A key type of its own says its hash with a member `u64 hash() const`.
template <class K> struct KeyHash<K, std::void_t<decltype(std::declval<const K&>().hash())>> {
  u64 operator()(const K& key) const { return key.hash(); }
};

namespace detail {
/// Where the entries of a map or set are by their keys: open addressing over a table whose size
/// is a power of two, each slot 0 (free) or the place of an entry + 1. Built anew when the
/// entries move (an entry taken out) or the table grows.
class KeyIndex {
 public:
  template <class Key, class KeyOf> std::size_t find(const Key& key, u64 hash, const KeyOf& key_of) const {
    if (slots_.empty()) return npos;
    const std::size_t mask = slots_.size() - 1;
    for (std::size_t s = static_cast<std::size_t>(hash) & mask;; s = (s + 1) & mask) {
      const u32 at = slots_[s];
      if (at == 0) return npos;
      if (key_of(at - 1) == key) return at - 1;
    }
  }
  /// Before an entry is added: room for `count` entries (the table at most half full).
  template <class HashOf> void room(std::size_t count, const HashOf& hash_of) {
    if (count * 2 <= slots_.size()) return;
    std::size_t size = 16;
    while (size < count * 2) size *= 2;
    rebuild(size, count - 1, hash_of);
  }
  /// The entry at `at` is there now (its key is new).
  void add(std::size_t at, u64 hash) {
    const std::size_t mask = slots_.size() - 1;
    std::size_t s = static_cast<std::size_t>(hash) & mask;
    while (slots_[s] != 0) s = (s + 1) & mask;
    slots_[s] = static_cast<u32>(at + 1);
  }
  /// The entries are `count`, each where it is now.
  template <class HashOf> void rebuild(std::size_t size, std::size_t count, const HashOf& hash_of) {
    slots_.assign(size, 0);
    for (std::size_t at = 0; at < count; ++at) add(at, hash_of(at));
  }
  template <class HashOf> void moved(std::size_t count, const HashOf& hash_of) { rebuild(slots_.size(), count, hash_of); }
  void clear() { slots_.assign(slots_.size(), 0); }
  void reserve(std::size_t count) {
    if (count * 2 > slots_.size()) {
      std::size_t size = 16;
      while (size < count * 2) size *= 2;
      slots_.reserve(size);
    }
  }
  static constexpr std::size_t npos = static_cast<std::size_t>(-1);

 private:
  std::vector<u32> slots_;
};
}  // namespace detail

/// A map whose entries stand in the order their keys came (see above). As std::unordered_map
/// for what a program does with it - find, [], try_emplace, insert_or_assign, erase, a loop over
/// `auto& [key, value]` - and, for a Blueprint, its entries by their place (key_at, value_at,
/// index_of). A key is not changed where it stands.
template <class K, class V, class Hash = KeyHash<K>> class Map {
 public:
  using key_type = K;
  using mapped_type = V;
  using value_type = std::pair<K, V>;
  using iterator = value_type*;
  using const_iterator = const value_type*;
  static constexpr std::size_t npos = detail::KeyIndex::npos;

  Map() = default;
  /// (a key said twice: its first entry - as std::unordered_map takes it)
  Map(std::initializer_list<value_type> entries) {
    reserve(entries.size());
    for (const value_type& e : entries) try_emplace(e.first, e.second);
  }
  Map(const Map& o) : entries_(o.entries_) { index(); }
  Map(Map&& o) noexcept : entries_(std::move(o.entries_)), index_(std::move(o.index_)) { o.entries_.clear(), o.index_ = {}; }
  Map& operator=(const Map& o) {
    if (this != &o) entries_ = o.entries_, index();
    return *this;
  }
  Map& operator=(Map&& o) noexcept {
    if (this != &o) entries_ = std::move(o.entries_), index_ = std::move(o.index_), o.entries_.clear(), o.index_ = {};
    return *this;
  }

  std::size_t size() const { return entries_.size(); }
  bool empty() const { return entries_.empty(); }
  iterator begin() { return entries_.data(); }
  iterator end() { return entries_.data() + entries_.size(); }
  const_iterator begin() const { return entries_.data(); }
  const_iterator end() const { return entries_.data() + entries_.size(); }

  /// The place of the key's entry among them, npos: there is none.
  std::size_t index_of(const K& key) const {
    return index_.find(key, hash_(key), [this](std::size_t at) -> const K& { return entries_[at].first; });
  }
  iterator find(const K& key) {
    const std::size_t at = index_of(key);
    return at == npos ? end() : begin() + at;
  }
  const_iterator find(const K& key) const {
    const std::size_t at = index_of(key);
    return at == npos ? end() : begin() + at;
  }
  bool contains(const K& key) const { return index_of(key) != npos; }
  std::size_t count(const K& key) const { return contains(key) ? 1 : 0; }

  /// The key's value - made with V's zero when there is none (std::unordered_map's []).
  V& operator[](const K& key) { return try_emplace(key).first->second; }
  /// Puts the key in with a value made of `args` - unless it is there (then nothing changes).
  /// (where its entry is, was it new)
  template <class... A> std::pair<iterator, bool> try_emplace(const K& key, A&&... args) {
    const u64 h = hash_(key);
    const std::size_t at = index_.find(key, h, [this](std::size_t k) -> const K& { return entries_[k].first; });
    if (at != npos) return {begin() + at, false};
    index_.room(entries_.size() + 1, [this](std::size_t k) { return hash_(entries_[k].first); });
    entries_.emplace_back(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(std::forward<A>(args)...));
    index_.add(entries_.size() - 1, h);
    return {end() - 1, true};
  }
  template <class W> std::pair<iterator, bool> emplace(const K& key, W&& value) { return try_emplace(key, std::forward<W>(value)); }
  std::pair<iterator, bool> insert(const value_type& entry) { return try_emplace(entry.first, entry.second); }
  std::pair<iterator, bool> insert(value_type&& entry) { return try_emplace(entry.first, std::move(entry.second)); }
  /// Puts the key in with the value - or gives the entry that is there the value.
  template <class W> std::pair<iterator, bool> insert_or_assign(const K& key, W&& value) {
    auto [it, fresh] = try_emplace(key);
    it->second = std::forward<W>(value);
    return {it, fresh};
  }

  /// Takes the key's entry out (the others keep their order). How many were taken out: 0 or 1.
  std::size_t erase(const K& key) {
    const std::size_t at = index_of(key);
    if (at == npos) return 0;
    erase_at(at);
    return 1;
  }
  /// Takes this entry out: the one after it is where it was.
  iterator erase(const_iterator it) {
    const std::size_t at = static_cast<std::size_t>(it - begin());
    erase_at(at);
    return begin() + at;
  }
  /// Takes the entry at this place out.
  void erase_at(std::size_t at) {
    entries_.erase(entries_.begin() + static_cast<std::ptrdiff_t>(at));
    index_.moved(entries_.size(), [this](std::size_t k) { return hash_(entries_[k].first); });
  }
  void clear() {
    entries_.clear();
    index_.clear();
  }
  void reserve(std::size_t count) {
    entries_.reserve(count);
    index_.reserve(count);
  }

  /// The entries by their place, in the order their keys came.
  const K& key_at(std::size_t at) const { return entries_[at].first; }
  V& value_at(std::size_t at) { return entries_[at].second; }
  const V& value_at(std::size_t at) const { return entries_[at].second; }

  /// The same entries in the same order.
  friend bool operator==(const Map& a, const Map& b) { return a.entries_ == b.entries_; }

 private:
  void index() {
    index_ = {};
    if (entries_.empty()) return;
    index_.room(entries_.size(), [this](std::size_t k) { return hash_(entries_[k].first); });
    index_.moved(entries_.size(), [this](std::size_t k) { return hash_(entries_[k].first); });
  }
  std::vector<value_type> entries_;
  detail::KeyIndex index_;
  [[no_unique_address]] Hash hash_{};
};

/// A set whose keys stand in the order they came (see above). As std::unordered_set for what a
/// program does with it - insert, find, count, erase, a loop over its keys - and, for a
/// Blueprint, its keys by their place (key_at, index_of).
template <class K, class Hash = KeyHash<K>> class Set {
 public:
  using key_type = K;
  using value_type = K;
  using iterator = const K*;
  using const_iterator = const K*;
  static constexpr std::size_t npos = detail::KeyIndex::npos;

  Set() = default;
  Set(std::initializer_list<K> keys) {
    reserve(keys.size());
    for (const K& k : keys) insert(k);
  }
  template <class It> Set(It first, It last) {
    for (; first != last; ++first) insert(*first);
  }
  Set(const Set& o) : keys_(o.keys_) { index(); }
  Set(Set&& o) noexcept : keys_(std::move(o.keys_)), index_(std::move(o.index_)) { o.keys_.clear(), o.index_ = {}; }
  Set& operator=(const Set& o) {
    if (this != &o) keys_ = o.keys_, index();
    return *this;
  }
  Set& operator=(Set&& o) noexcept {
    if (this != &o) keys_ = std::move(o.keys_), index_ = std::move(o.index_), o.keys_.clear(), o.index_ = {};
    return *this;
  }

  std::size_t size() const { return keys_.size(); }
  bool empty() const { return keys_.empty(); }
  const_iterator begin() const { return keys_.data(); }
  const_iterator end() const { return keys_.data() + keys_.size(); }

  std::size_t index_of(const K& key) const {
    return index_.find(key, hash_(key), [this](std::size_t at) -> const K& { return keys_[at]; });
  }
  const_iterator find(const K& key) const {
    const std::size_t at = index_of(key);
    return at == npos ? end() : begin() + at;
  }
  bool contains(const K& key) const { return index_of(key) != npos; }
  std::size_t count(const K& key) const { return contains(key) ? 1 : 0; }

  /// Puts the key in unless it is there. (where it is, was it new)
  std::pair<const_iterator, bool> insert(const K& key) {
    const u64 h = hash_(key);
    const std::size_t at = index_.find(key, h, [this](std::size_t k) -> const K& { return keys_[k]; });
    if (at != npos) return {begin() + at, false};
    index_.room(keys_.size() + 1, [this](std::size_t k) { return hash_(keys_[k]); });
    keys_.push_back(key);
    index_.add(keys_.size() - 1, h);
    return {end() - 1, true};
  }
  std::pair<const_iterator, bool> emplace(const K& key) { return insert(key); }
  template <class It> void insert(It first, It last) {
    for (; first != last; ++first) insert(*first);
  }

  std::size_t erase(const K& key) {
    const std::size_t at = index_of(key);
    if (at == npos) return 0;
    erase_at(at);
    return 1;
  }
  const_iterator erase(const_iterator it) {
    const std::size_t at = static_cast<std::size_t>(it - begin());
    erase_at(at);
    return begin() + at;
  }
  void erase_at(std::size_t at) {
    keys_.erase(keys_.begin() + static_cast<std::ptrdiff_t>(at));
    index_.moved(keys_.size(), [this](std::size_t k) { return hash_(keys_[k]); });
  }
  void clear() {
    keys_.clear();
    index_.clear();
  }
  void reserve(std::size_t count) {
    keys_.reserve(count);
    index_.reserve(count);
  }

  const K& key_at(std::size_t at) const { return keys_[at]; }

  friend bool operator==(const Set& a, const Set& b) { return a.keys_ == b.keys_; }

 private:
  void index() {
    index_ = {};
    if (keys_.empty()) return;
    index_.room(keys_.size(), [this](std::size_t k) { return hash_(keys_[k]); });
    index_.moved(keys_.size(), [this](std::size_t k) { return hash_(keys_[k]); });
  }
  std::vector<K> keys_;
  detail::KeyIndex index_;
  [[no_unique_address]] Hash hash_{};
};

}  // namespace kern
