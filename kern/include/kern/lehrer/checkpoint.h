#pragma once
#include "kern/core/base.h"
#include <cstring>
#include <type_traits>
#include <vector>

namespace kern::lehrer::checkpoint {
struct Writer {
  std::vector<u8> bytes;
  template<class T> void scalar(const T& value) {
    static_assert(std::is_arithmetic_v<T>);
    const auto* p = reinterpret_cast<const u8*>(&value);
    bytes.insert(bytes.end(),p,p+sizeof(T));
  }
  template<class T> void vector(const std::vector<T>& value) {
    static_assert(std::is_arithmetic_v<T>);
    scalar(static_cast<u64>(value.size()));
    if(!value.empty()){
      const auto* p=reinterpret_cast<const u8*>(value.data());
      bytes.insert(bytes.end(),p,p+value.size()*sizeof(T));
    }
  }
};
struct Reader {
  const u8* data;
  std::size_t size, at=0;
  template<class T> bool scalar(T& value) {
    static_assert(std::is_arithmetic_v<T>);
    if(at>size || sizeof(T)>size-at)return false;
    std::memcpy(&value,data+at,sizeof(T));at+=sizeof(T);return true;
  }
  template<class T> bool vector(std::vector<T>& value, std::size_t max) {
    static_assert(std::is_arithmetic_v<T>);
    u64 n=0;
    if(!scalar(n)||n>max||n>(size-at)/sizeof(T))return false;
    value.resize(static_cast<std::size_t>(n));
    if(n)std::memcpy(value.data(),data+at,value.size()*sizeof(T));
    at+=value.size()*sizeof(T);return true;
  }
  bool end() const { return at==size; }
};
} // namespace kern::lehrer::checkpoint
