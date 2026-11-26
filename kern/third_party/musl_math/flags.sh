# The one list of the switches that keep a compiler from computing the engine's mathematics itself (sourced by every build
# of kern and its games: kern/docs/blueprint_cpp.md "Mathe-Funktionen"). src/std_names.c gives these functions under their
# standard names - musl's, as kern::dm has them; these switches make every call of them a call (no folding of sin(0.5) at
# compile time, no pow(x, 2) as x*x). GCC folds __builtin_sinf (what libstdc++'s std::sin(float) is) all the same: with it
# -frounding-math as well, which keeps it from folding an inexact result.
#   kern_math_flags <compiler>   the switches for that compiler (C or C++)
kern_math_flags() {
  local f out=""
  for f in sin cos tan asin acos atan atan2 exp expm1 log log10 pow hypot sincos; do out+=" -fno-builtin-$f -fno-builtin-${f}f"; done
  if "$1" --version 2>/dev/null | head -1 | grep -qiE 'g\+\+|gcc' && ! "$1" --version 2>/dev/null | grep -qi clang; then out+=" -frounding-math"; fi
  echo "$out"
}
