#!/usr/bin/env python3
"""Writes kern/tests/elementary_reference.h: log2, log1p, exp2, cbrt, acosh and atanh of a set of f64 and f32 inputs,
computed with Python's decimal at 200 digits from the definitions, and ldexp and remainder exactly (Python's Fraction,
rounded to the nearest double or float by the rule of IEEE 754 written out here) - independent of the engine's
mathematics (kern/src/math/elementary.c, hyperbolic.c) and of any libm. An accurate value is given as hi, the nearest
double, and the rest in ulps of hi, so that the test can say an error in fractions of an ulp (also where the value is
subnormal); an exact one as the one double (or float) it must be.

    python3 kern/tests/elementary_reference.py > kern/tests/elementary_reference.h
"""
import math
import struct
from decimal import Decimal as D, getcontext
from fractions import Fraction as Q

getcontext().prec = 200
getcontext().Emax = 10**9
getcontext().Emin = -10**9

FNS = ("log2", "log1p", "exp2", "cbrt", "acosh", "atanh")
LN2 = D(2).ln()


def ln1p(x):
    """ln(1 + x) for |x| small without forming 1 + x (which 200 digits do not hold for x = 1e-300): the series."""
    if abs(x) < D("1e-20"):
        s, t, k = D(0), x, 1
        while k < 40:
            s += t / k if k % 2 else -t / k
            t *= x
            k += 1
        return s
    return (1 + x).ln()


def reference(fn, x):
    """The exact value (to 200 digits) of fn at the double x; None where it is not a real number or beyond every double."""
    X = D(x)
    if fn == "log2":
        return None if X <= 0 else X.ln() / LN2
    if fn == "log1p":
        return None if X <= -1 else ln1p(X)
    if fn == "exp2":
        return None if X > 1100 else (X * LN2).exp()
    if fn == "cbrt":
        if X == 0:
            return D(0)
        r = (abs(X).ln() / 3).exp()
        return r if X > 0 else -r
    if fn == "acosh":
        if X < 1:
            return None
        return ln1p((X - 1) + ((X - 1) * (X + 1)).sqrt())
    if fn == "atanh":
        if abs(X) >= 1:
            return None
        return (ln1p(X) - ln1p(-X)) / 2


def hi_rest(r):
    """hi, the nearest double, and the rest r - hi in ulps of hi (as a double has them: 2^(e - 52), e >= -1022) - in ulps,
    not as a double of its own, which below 2^-1022 would itself be rounded to a subnormal."""
    hi = float(r)
    if math.isinf(hi):
        return None
    e = math.frexp(hi)[1] - 1 if hi != 0 else -1022
    return hi, float((r - D(hi)) / D(2) ** (max(e, -1022) - 52))


def round_to(v, mant, emin, emax):
    """The rational v rounded to the nearest number with `mant` bits of mantissa, exponents down to emin (subnormal below:
    multiples of 2^(emin - mant + 1)) and below 2^(emax + 1) - a tie to the even one; beyond: +-inf. As a Python float."""
    if v == 0:
        return 0.0
    sign = -1 if v < 0 else 1
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()   # 2^e <= a < 2^(e+1), after the fix below
    if Q(2) ** e > a:
        e -= 1
    if Q(2) ** (e + 1) <= a:
        e += 1
    q = max(e, emin) - mant + 1                                  # the step of the grid at a
    n = a / Q(2) ** q
    k = n.numerator // n.denominator
    rest = n - k
    if rest > Q(1, 2) or (rest == Q(1, 2) and k % 2 == 1):
        k += 1
    r = Q(k) * Q(2) ** q
    if r >= Q(2) ** (emax + 1):
        return sign * math.inf
    return sign * float(r)


def to_double(v):
    return round_to(v, 53, -1022, 1023)


def to_float(v):
    return round_to(v, 24, -126, 127)


def f32(x):
    try:
        return struct.unpack("f", struct.pack("f", x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def drawn(seed, count, lo_exp, hi_exp, signed=True):
    s, out = seed, []
    for _ in range(count):
        s = (s * 6364136223846793005 + 1442695040888963407) % (1 << 64)
        u = (s >> 11) / 9007199254740992.0
        e = lo_exp + (s % (hi_exp - lo_exp + 1))
        v = (u if not signed else u * 2 - 1) * 2.0 ** e
        out.append(v)
    return out


def inputs(fn, single):
    tiny = [2.0 ** -1074, 2.0 ** -1060, 2.0 ** -1022, 2.0 ** -600, 1e-300, 1e-100, 2.0 ** -60, 2.0 ** -30, 2.0 ** -28, 2.0 ** -27, 1e-8, 1e-5, 1e-3]
    if single:
        tiny = [2.0 ** -149, 2.0 ** -140, 2.0 ** -126, 2.0 ** -100, 1e-30, 2.0 ** -60, 2.0 ** -30, 2.0 ** -28, 2.0 ** -27, 1e-8, 1e-5, 1e-3]
    near1 = []
    for k in (1, 2, 3, 10, 20, 26, 27, 28, 29, 40, 52, 53):
        near1 += [1 + 2.0 ** -k, 1 - 2.0 ** -k]
    if fn == "log2":
        v = tiny + near1 + [0.5, 0.7071067811865475, 0.7071067811865476, 1.4142135623730950, 1.4142135623730951, 3.0, 10.0, 1e10, 1e100, 1e300,
                            1.7976931348623157e308, 2.0 ** 1023 * 1.5]
        v += [abs(x) for x in drawn(0x9E3779B97F4A7C15, 160, -1074 if not single else -149, 1023 if not single else 127)]
        v += [1 + x for x in drawn(0x1234567, 60, -50, -1)]
    elif fn == "log1p":
        v = tiny + [-t for t in tiny] + [-1 + 2.0 ** -k for k in (1, 2, 10, 30, 52, 53)] + [0.5, 1.0, 1.5, 2.0, 3.2, 10.0, 1e10, 2.0 ** 53, 1e100, 1e300]
        v += drawn(0x2545F4914F6CDD1D, 120, -60, 0)
        v += [abs(x) for x in drawn(0x5DEECE66D, 60, -10, 1000 if not single else 120)]
        v += [x for x in drawn(0xABCDEF, 40, -1, -1) if x > -1]
    elif fn == "exp2":
        v = [0.0, 0.5, -0.5, 1.0, -1.0, 0.25, 1e-20, -1e-20, 1e-5, 10.0, 100.5, -100.5, 1023.0, 1023.5, 1023.99, -1021.5, -1022.0, -1022.5, -1050.3,
             -1073.0, -1074.0, -1074.5, -1074.9, -1075.0 + 2.0 ** -42, 0.3, -0.3]
        if single:
            v = [0.0, 0.5, -0.5, 1.0, -1.0, 0.25, 1e-20, -1e-20, 1e-5, 10.0, 100.5, -100.5, 127.0, 127.5, 127.99, -125.5, -126.0, -126.5, -140.3,
                 -148.0, -149.0, -149.5, -149.9, 0.3, -0.3]
        v += drawn(0x9E3779B97F4A7C15, 160, -30, 9)
        v += drawn(0x77777777, 60, -1, 0)
    elif fn == "cbrt":
        v = tiny + [-t for t in tiny] + [0.125, 0.5, 2.0, 3.0, 4.0, 7.0, 8.0, 9.0, 27.0, 1000.0, 1e10, 1e100, 1e300, 1.7976931348623157e308, -2.0, -27.5]
        if single:
            v = [x for x in v if abs(x) < 3e38]
        v += drawn(0x9E3779B97F4A7C15, 200, -1074 if not single else -149, 1023 if not single else 127)
    elif fn == "acosh":
        v = [1.0] + [x for x in near1 if x > 1] + [1.5, 2.0, 2.0000000000000004, 1.9999999999999998, 2.5, 10.0, 2.0 ** 28, 2.0 ** 28 + 2.0 ** -24,
                                                  2.0 ** 28 - 2.0 ** -25, 1e10, 1e100, 1e300, 1.7976931348623157e308]
        v += [1 + abs(x) for x in drawn(0x9E3779B97F4A7C15, 120, -52, 0)]
        v += [1 + abs(x) for x in drawn(0x31415926, 100, 0, 1020 if not single else 120)]
    elif fn == "atanh":
        v = tiny + [-t for t in tiny] + [0.25, 0.4999999999999999, 0.5, 0.5000000000000001, 0.75, 0.9, 0.99]
        v += [1 - 2.0 ** -k for k in (2, 5, 10, 20, 30, 40, 52, 53)]
        v += drawn(0x9E3779B97F4A7C15, 200, -40, -1)
        v = v + [-x for x in v[:30]]
    out = []
    for x in v:
        y = f32(x) if single else x
        if math.isinf(y) or (single and y == 0 and x != 0):
            continue
        out.append(y)
    return out


def accurate_rows(single):
    rows = []
    for k, fn in enumerate(FNS):
        for x in inputs(fn, single):
            r = reference(fn, x)
            if r is None:
                continue
            if single and abs(r) > D("3.4028234663852886e38"):
                continue
            hl = hi_rest(r)
            if hl is None:
                continue
            rows.append(f"{{{k}, {x.hex()}, {hl[0].hex()}, {hl[1].hex()}}}")
    return rows


def hexs(v):
    if math.isinf(v):
        return "kInf" if v > 0 else "-kInf"
    if v == 0:
        return "-0.0" if math.copysign(1, v) < 0 else "0.0"
    return v.hex()


def exact_rows(single):
    """{fn (0 ldexp, 1 remainder), x, y (ldexp: n), the result}"""
    to = to_float if single else to_double
    r = f32 if single else (lambda t: t)
    rows = []
    big, small = (127, -149) if single else (1023, -1074)
    mant = 24 if single else 53
    # ldexp: specials, subnormal results with ties both ways, across the edges of overflow and of nothing
    xs = [1.0, -1.0, 3.0, 5.0, -7.0, 1.5, 1.75, 0.75, 2.0 ** small, -(2.0 ** small), 3 * 2.0 ** small, 2.0 ** (small + mant - 1),
          (2 - 2.0 ** (1 - mant)) * 2.0 ** big, 0.1, -0.3, 1e-30, 12345.678]
    xs += drawn(0x9E3779B97F4A7C15, 80, small + 10 if not single else -140, big - 10)
    ns = [0, 1, -1, 2, -2, 10, -10, small, small - 1, small - 2, small + 1, -mant - 1, big, big + 1, -big - mant, -big - mant - 1, 2 * big, -2 * big,
          3 * big, -3 * big, 100000, -100000]
    s = 0x2545F4914F6CDD1D
    for x in xs:
        x = r(x)
        if math.isinf(x):
            continue
        e = math.frexp(x)[1]
        cand = list(ns) + [small - e + k for k in (-3, -2, -1, 0, 1, 2, 10, 30)] + [big + 1 - e + k for k in (-1, 0, 1)]
        for _ in range(4):
            s = (s * 6364136223846793005 + 1442695040888963407) % (1 << 64)
            cand.append(int(s % (6 * big)) - 3 * big)
        for n in cand:
            want = to(Q(x) * Q(2) ** n) if x != 0 else x
            if want == 0:
                want = math.copysign(0.0, x)
            rows.append(f"{{0, {hexs(x)}, {n}.0, {hexs(want)}}}")
    # remainder: ties (k even and odd), quotients beyond 2^1000, subnormal divisors and remainders, signs, |x| < |y|
    pairs = [(2.5, 1.0), (3.5, 1.0), (-2.5, 1.0), (-3.5, 1.0), (2.5, -1.0), (1.0, 1.0), (-1.0, 1.0), (0.5, 1.0), (1.5, 1.0), (0.49, 1.0),
             (0.0, 1.0), (-0.0, 1.0), (5.0, 3.0), (7.0, 3.0), (-7.0, 3.0), (1e300, 3e-300), (1e300, 7.0), (3.0, 1e300), (6.0, 4.0), (10.0, 4.0),
             (2.0 ** big, 3.0), (2.0 ** small * 7, 2.0 ** small * 2), (2.0 ** small * 5, 2.0 ** small * 2), (2.0 ** small * 3, 2.0 ** small),
             ((2 - 2.0 ** (1 - mant)) * 2.0 ** big, (2 - 2.0 ** (1 - mant)) * 2.0 ** (big - 1) * 1.25),
             ((2 - 2.0 ** (1 - mant)) * 2.0 ** big, 1.5 * 2.0 ** big), (1.5 * 2.0 ** big, (2 - 2.0 ** (1 - mant)) * 2.0 ** big),
             (360.5, 360.0), (-725.0, 360.0), (540.0, 360.0), (900.0, 360.0), (1e-300, 1e300), (6.283185307179586, 3.141592653589793)]
    xa = drawn(0x9E3779B97F4A7C15, 220, small + 30 if not single else -120, big)
    ya = drawn(0x5DEECE66D, 220, small if not single else -149, big)
    pairs += list(zip(xa, ya))
    xb = drawn(0x31415926, 160, -20, 40)
    yb = drawn(0x27182818, 160, -20, 20)
    pairs += list(zip(xb, yb))
    for x, y in pairs:
        x, y = r(x), r(y)
        if y == 0 or math.isinf(x) or math.isinf(y):
            continue
        qx, qy = Q(x), Q(y)
        t = qx / qy
        k = math.floor(t)
        if t - k > Q(1, 2) or (t - k == Q(1, 2) and k % 2 == 1):
            k += 1
        exact = qx - k * qy
        want = float(exact)
        if to(exact) != want:
            raise SystemExit(f"remainder({x}, {y}) is not a {'float' if single else 'double'}")
        if want == 0:
            want = math.copysign(0.0, x)
        rows.append(f"{{1, {hexs(x)}, {hexs(y)}, {hexs(want)}}}")
    return rows


def check_rounding():
    """round_to agrees with Python's own conversion of a fraction (CPython rounds a / b correctly) for doubles."""
    s = 1
    for _ in range(2000):
        s = (s * 6364136223846793005 + 1442695040888963407) % (1 << 64)
        v = Q(s >> 3, (s % 1000003) + 1) * Q(2) ** (int(s % 2200) - 1150)
        try:
            want = float(v)
        except OverflowError:
            want = math.inf
        assert to_double(v) == want, v


def main():
    check_rounding()
    print("// Written by kern/tests/elementary_reference.py - do not edit: log2, log1p, exp2, cbrt, acosh, atanh at 200 digits (Python")
    print("// decimal), ldexp and remainder exact (Python Fraction) - independent of the engine's mathematics.")
    print("#pragma once")
    print("namespace elementary_reference {")
    print("inline constexpr double kInf = __builtin_huge_val();")
    print("// fn: 0 log2, 1 log1p, 2 exp2, 3 cbrt, 4 acosh, 5 atanh; the value is hi + rest ulps of hi (ulp: 2^(e - 52), e >= -1022)")
    print("struct Row { int fn; double x, hi, rest; };")
    print("struct Exact { int fn; double x, y, want; };   // fn: 0 ldexp (y: n), 1 remainder; want: the one result (f32: a float)")
    for single in (False, True):
        for name, rows, kind in ((f"kAccurate{'Float' if single else 'Double'}", accurate_rows(single), "Row"),
                                 (f"kExact{'Float' if single else 'Double'}", exact_rows(single), "Exact")):
            print(f"inline constexpr {kind} {name}[] = {{")
            for i in range(0, len(rows), 3):
                print("    " + ", ".join(rows[i:i + 3]) + ",")
            print("};")
    print("}  // namespace elementary_reference")


if __name__ == "__main__":
    main()
