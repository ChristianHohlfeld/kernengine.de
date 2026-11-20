// The fields of a plain struct of a program (an aggregate: public fields, no constructor of its own, no
// base), as references in the order of its declaration - kern::fields_of(s) is std::tie(s.a, s.b, ...).
// So code that hands a struct on field by field (a Blueprint's call: kern/logic/actor_blueprint.h)
// is written once for every such struct, with no list of its fields kept beside it. It costs what
// naming the fields costs: a structured binding, nothing at run time.
#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace kern {

namespace detail {
/// Stands for the value of any one field where the struct is made from a braced list.
struct AnyField {
  template <class T> constexpr operator T&() const&& noexcept;
};
template <class T, class... F> constexpr std::size_t field_count() {
  if constexpr (requires { T{F{}..., AnyField{}}; }) return field_count<T, F..., AnyField>();
  else return sizeof...(F);
}
}  // namespace detail

/// Is T a struct whose fields fields_of() names: an aggregate class, no union, of at most 32 fields.
template <class T>
concept PlainStruct = std::is_class_v<T> && std::is_aggregate_v<T> && !std::is_union_v<T> && detail::field_count<T>() >= 1 && detail::field_count<T>() <= 32;

/// The number of fields of a plain struct.
template <PlainStruct T> inline constexpr std::size_t field_count = detail::field_count<T>();

/// The fields of `s`, in their order: std::tie(s.a, s.b, ...) (const where s is).
template <class S>
  requires PlainStruct<std::remove_const_t<S>>
constexpr auto fields_of(S& s) {
  constexpr std::size_t n = field_count<std::remove_const_t<S>>;
  if constexpr (n == 1) {
    auto& [f0] = s;
    return std::tie(f0);
  }
  else if constexpr (n == 2) {
    auto& [f0, f1] = s;
    return std::tie(f0, f1);
  }
  else if constexpr (n == 3) {
    auto& [f0, f1, f2] = s;
    return std::tie(f0, f1, f2);
  }
  else if constexpr (n == 4) {
    auto& [f0, f1, f2, f3] = s;
    return std::tie(f0, f1, f2, f3);
  }
  else if constexpr (n == 5) {
    auto& [f0, f1, f2, f3, f4] = s;
    return std::tie(f0, f1, f2, f3, f4);
  }
  else if constexpr (n == 6) {
    auto& [f0, f1, f2, f3, f4, f5] = s;
    return std::tie(f0, f1, f2, f3, f4, f5);
  }
  else if constexpr (n == 7) {
    auto& [f0, f1, f2, f3, f4, f5, f6] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6);
  }
  else if constexpr (n == 8) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7);
  }
  else if constexpr (n == 9) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8);
  }
  else if constexpr (n == 10) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9);
  }
  else if constexpr (n == 11) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10);
  }
  else if constexpr (n == 12) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11);
  }
  else if constexpr (n == 13) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12);
  }
  else if constexpr (n == 14) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13);
  }
  else if constexpr (n == 15) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14);
  }
  else if constexpr (n == 16) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15);
  }
  else if constexpr (n == 17) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16);
  }
  else if constexpr (n == 18) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17);
  }
  else if constexpr (n == 19) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18);
  }
  else if constexpr (n == 20) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19);
  }
  else if constexpr (n == 21) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20);
  }
  else if constexpr (n == 22) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21);
  }
  else if constexpr (n == 23) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22);
  }
  else if constexpr (n == 24) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23);
  }
  else if constexpr (n == 25) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24);
  }
  else if constexpr (n == 26) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25);
  }
  else if constexpr (n == 27) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26);
  }
  else if constexpr (n == 28) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27);
  }
  else if constexpr (n == 29) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28);
  }
  else if constexpr (n == 30) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29);
  }
  else if constexpr (n == 31) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30);
  }
  else if constexpr (n == 32) {
    auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31] = s;
    return std::tie(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31);
  }
}

}  // namespace kern
