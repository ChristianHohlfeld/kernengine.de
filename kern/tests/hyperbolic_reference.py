#!/usr/bin/env python3
"""Writes kern/tests/hyperbolic_reference.h: sinh, cosh, tanh and asinh of a set of f64 and f32 inputs, computed with
Python's decimal at 200 digits from the definitions - independent of the engine's mathematics (kern/src/math/hyperbolic.c)
and of any libm. Each value is given as two doubles, hi + lo (hi the nearest double, lo the rest), so that the test can
say an error in fractions of an ulp.

    python3 kern/tests/hyperbolic_reference.py > kern/tests/hyperbolic_reference.h
"""
import math
from decimal import Decimal as D, getcontext

getcontext().prec = 200
getcontext().Emax = 10**9
getcontext().Emin = -10**9


def series_sinh(x):
    s, t = x, x
    for i in range(1, 60):
        t = t * x * x / ((2 * i) * (2 * i + 1))
        s += t
    return s


def reference(fn, x):
    """The exact value (to 200 digits) of fn at the double x, or None where it is beyond every double."""
    X = D(x)
    if fn in ("sinh", "cosh") and abs(X) > 800:   # (beyond every double: e^800 / 2 > 2^1024)
        return D("1e400") if fn == "cosh" or X > 0 else D("-1e400")
    if fn == "sinh":
        return series_sinh(X) if abs(X) < D("1e-3") else (X.exp() - (-X).exp()) / 2
    if fn == "cosh":
        return (X.exp() + (-X).exp()) / 2
    if fn == "tanh":
        if abs(X) < D("1e-3"):
            return series_sinh(X) / ((X.exp() + (-X).exp()) / 2)
        if abs(X) > 400:
            return D(1) if X > 0 else D(-1)
        e = (2 * X).exp()
        return (e - 1) / (e + 1)
    if fn == "asinh":
        A = abs(X)
        if A < D("1e-3"):   # (the series: A - A^3/6 + 3A^5/40 - ...)
            r, term, k = D(0), A, 0
            while k < 80:
                c = D((-1) ** k) * D(math.factorial(2 * k)) / (D(4) ** k * D(math.factorial(k)) ** 2 * (2 * k + 1))
                r += c * A ** (2 * k + 1)
                k += 1
        else:
            r = (A + (A * A + 1).sqrt()).ln()
        return r if X >= 0 else -r


def hi_lo(r):
    hi = float(r)
    if math.isinf(hi):
        return None
    lo = float(r - D(hi))
    return hi, lo


def inputs(single):
    lim = 89.0 if single else 710.0
    v = [0.5, 1.0, 2.0, 3.0, 0.25, 0.1, 1e-3, 1e-5, 1e-8, 2.0 ** -27, 2.0 ** -29, 0.34, 0.35, 0.54, 0.56, 1.5, 4.0, 10.0, 19.9, 20.1, 21.9,
         22.1, 30.0, 50.0, 88.0, 88.7, 300.0, 700.0, 709.5, 710.0, 2.0 ** 27, 2.0 ** 29, 1e10, 1e100, 1e300]
    s = 0x9E3779B97F4A7C15
    for i in range(120):   # (a series of drawn values: magnitudes 2^-30 .. 2^10, both signs)
        s = (s * 6364136223846793005 + 1442695040888963407) % (1 << 64)
        u = (s >> 11) / 9007199254740992.0
        v.append((u * 2 - 1) * 2.0 ** ((s % 41) - 30))
    out = []
    for x in v:
        for y in (x, -x):
            out.append(_f32(y) if single else y)
    return out


def _f32(x):
    import struct
    try:
        return struct.unpack("f", struct.pack("f", x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def main():
    print("// Written by kern/tests/hyperbolic_reference.py - do not edit: sinh, cosh, tanh, asinh at 200 digits (Python decimal),")
    print("// independent of the engine's mathematics. {function, the input, the value as hi + lo}.")
    print("#pragma once")
    print("namespace hyperbolic_reference {")
    print("struct Row { int fn; double x, hi, lo; };   // fn: 0 sinh, 1 cosh, 2 tanh, 3 asinh")
    for single in (False, True):
        name = "kFloat" if single else "kDouble"
        rows = []
        for x in inputs(single):
            if math.isinf(x):
                continue
            for k, fn in enumerate(("sinh", "cosh", "tanh", "asinh")):
                r = reference(fn, x)
                if single and abs(r) > D("3.4028234663852886e38"):
                    continue
                hl = hi_lo(r)
                if hl is None:
                    continue
                rows.append(f"{{{k}, {x.hex()}, {hl[0].hex()}, {hl[1].hex()}}}")
        print(f"inline constexpr Row {name}[] = {{")
        for i in range(0, len(rows), 3):
            print("    " + ", ".join(rows[i:i + 3]) + ",")
        print("};")
    print("}  // namespace hyperbolic_reference")


if __name__ == "__main__":
    main()
