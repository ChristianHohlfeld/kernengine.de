# The one list of the switches that keep a compiler from computing the engine's mathematics itself (sourced by every build
# of kern and its games: kern/docs/blueprint_cpp.md "Mathe-Funktionen"). src/std_names.c gives these functions under their
# standard names - musl's, as kern::dm has them; these switches make every call of them a call (no folding of sin(0.5) at
# compile time, no pow(x, 2) as x*x - see -fmath-errno below). GCC folds __builtin_sinf (what libstdc++'s std::sin(float) is) all the same: with it
# -frounding-math as well, which keeps it from folding an inexact result, and -fdisable-tree-powcabs (below).
#   kern_math_flags <compiler>   the switches for that compiler (C or C++)
kern_math_flags() {
  local f out=""
  for f in sin cos tan asin acos atan atan2 exp expm1 log log10 pow hypot sincos; do out+=" -fno-builtin-$f -fno-builtin-${f}f"; done
  # (-fmath-errno: without it clang makes __builtin_pow - what libc++'s std::pow is, in WebAssembly - an intrinsic and pow(x, 2)
  # x * x, whatever -fno-builtin-pow says; belegt by CI 45810300304, math_names_wasm_test. It comes after a -fno-math-errno of a
  # build: these switches stand behind the others.)
  out+=" -fmath-errno"
  # GCC: told by what the compiler says it is (__GNUC__ without __clang__), not by its name - `c++` and `cc` are GCC on many
  # systems, and "c++ (Ubuntu 13.3.0 ...)" names neither g++ nor gcc (a run there folded std::atan2(1.5f, -2.f), 1 ulp off).
  # -fdisable-tree-powcabs: GCC's pass that makes __builtin_powf(x, 2) - libstdc++'s std::pow(float, float) - x * x whatever
  # the switches say; x * x rounds a tie to even, the engine's powf does not (90.109375f squared). A GCC without that pass
  # stops the build here ("unknown pass"): there it is not shown that the mathematics is the engine's.
  # (GCC says "note: disable pass tree-powcabs ..." once per file it compiles.)
  local macros
  macros="$("$1" -dM -E -x c /dev/null 2>/dev/null)"
  if grep -q '__GNUC__' <<< "$macros" && ! grep -q '__clang__' <<< "$macros"; then out+=" -frounding-math -fdisable-tree-powcabs"; fi
  echo "$out"
}
