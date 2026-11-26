/* The engine's own ldexp, remainder, log2, log1p, exp2 and cbrt (f64 and f32). Kern's code under Kern's licence - NOT from
 * musl (kern/third_party/musl_math has none of these). Like hyperbolic.c: from the definitions, over what the engine already
 * has - musl's exp and log (kern_musl_*), sqrt - with nothing but + - * /, whole-number arithmetic on the bits and those:
 * the same bits on every target (x86, ARM, WebAssembly), built like musl_math (-ffp-contract=off). Every build takes this
 * file where it takes musl_math (kern/docs/blueprint_cpp.md "Mathe-Funktionen"); kern::dm, the Blueprint's functions and the
 * standard names below (std::ldexp ...) all call these.
 *
 * Exact - the one double (float) nearest the exact value, as IEEE 754 defines them:
 *   ldexp(x, n) = x * 2^n: on the bits - the exponent moved; where the result is subnormal the mantissa is shifted and
 *         rounded once (to nearest, a tie to even); beyond the largest double +-inf, below half the smallest +-0.
 *   remainder(x, y) = x - k * y, k the whole number nearest x / y (of two, the even one): a long division on the mantissas
 *         (the remainder of |x| by |y| and whether the quotient is odd) - always a number a double holds, no rounding.
 *         y = 0 or x = +-inf: NaN; y = +-inf: x; a zero result has the sign of x.
 * Accurate (bounds: kern/tests/elementary_reference.h, against values computed with 200 digits, the bound in the test):
 *   log2:  x = m * 2^k, m in [sqrt(1/2), sqrt 2): k + log(m) / ln 2, the product of log(m) and 1/ln 2 (as hi + lo) taken
 *          exactly (Veltkamp/Dekker) and added to k with its error - log2(2^k) is k exactly.
 *   log1p: |y| < 2^-29: its series to y^4. Else u = 1 + y, rounded; log(u) + (y - (u - 1)) / u - the error of the sum
 *          (exact: Fast2Sum) put back to first order.
 *   exp2:  x = k + f, k the whole number nearest x (f = x - k exact): exp(f * ln 2) * 2^k with the exact ldexp above -
 *          exp2(k) is 2^k exactly.
 *   cbrt:  x = m * 2^(3q), m in [1, 8): y = exp(log(m) / 3), then one Newton step y - (y^3 - m) / (3 y^2) with y^3 - m
 *          computed exactly (Dekker products), so y is within a hair of the nearest double; times 2^q.
 * NaN gives NaN; +-0 gives +-0 (exp2: 1, log2: -inf); outside the domain NaN (log2 and log1p of what is below 0 and -1).
 * The f32 functions compute in f64 and round once to f32 (ldexp and remainder: exact as well). */
#include <stdint.h>

double kern_musl_exp(double), kern_musl_log(double);
double kern_musl___math_invalid(double);    /* (musl_math: NaN the way musl's log makes it) */
double kern_musl___math_divzero(uint32_t);  /* (musl_math: -inf or +inf the way musl's log makes it) */

static uint64_t kern_bits(double x) { union { double d; uint64_t u; } v = {x}; return v.u; }
static double kern_double(uint64_t u) { union { uint64_t u; double d; } v = {u}; return v.d; }
static double kern_abs(double x) { return __builtin_fabs(x); }

/* a * b = hi + lo exactly (no overflow): Veltkamp's split, Dekker's product - + - * only. */
static void kern_two_prod(double a, double b, double* hi, double* lo) {
  const double c = 134217729.0;   /* 2^27 + 1 */
  double t = c * a;
  const double ah = t - (t - a), al = a - ah;
  t = c * b;
  const double bh = t - (t - b), bl = b - bh;
  *hi = a * b;
  *lo = ((ah * bh - *hi) + ah * bl + al * bh) + al * bl;
}

double kern_math_ldexp(double x, int n) {
  const uint64_t u = kern_bits(x);
  const uint64_t sign = u & 0x8000000000000000ull;
  int e = (int)((u >> 52) & 0x7ff);
  uint64_t m = u & 0x000fffffffffffffull;
  if (e == 0x7ff) return x + x;                  /* NaN, +-inf */
  if (e == 0 && m == 0) return x;                /* +-0 */
  if (n == 0) return x;
  /* |x| = m * 2^(E - 1075), m in [2^52, 2^53) */
  long E;
  if (e == 0) {                                  /* subnormal: made normal (E = 1 - leading zeros) */
    E = 1;
    while (!(m & 0x0010000000000000ull)) m <<= 1, --E;
  } else {
    E = e;
    m |= 0x0010000000000000ull;
  }
  E += n;                                        /* (long: no overflow, |n| < 2^31) */
  if (E >= 0x7ff) return kern_double(sign | 0x7ff0000000000000ull);   /* beyond the largest double: inf */
  if (E >= 1) return kern_double(sign | ((uint64_t)E << 52) | (m & 0x000fffffffffffffull));
  /* subnormal: value = m * 2^(E - 1075) = (m >> s) * 2^-1074 with s = 1 - E, rounded to nearest, a tie to even */
  const long s = 1 - E;
  if (s > 53) return kern_double(sign);          /* below half the smallest subnormal (m < 2^53): +-0 */
  uint64_t q = m >> s;
  const uint64_t rest = m & ((1ull << s) - 1), half = 1ull << (s - 1);
  if (rest > half || (rest == half && (q & 1))) ++q;   /* (q may become 2^52: the smallest normal, its bits are right) */
  return kern_double(sign | q);
}

float kern_math_ldexpf(float x, int n) {
  /* In f64 exactly (|x| * 2^n for |n| <= 400 is a normal double with x's 24 bits), rounded once to f32 - also where it is
   * subnormal in f32 or beyond it (the conversion rounds as IEEE says). Beyond +-400 the result is 0 or inf either way. */
  if (n > 400) n = 400;
  if (n < -400) n = -400;
  return (float)kern_math_ldexp((double)x, n);
}

/* |x| = m * 2^p, m in [2^52, 2^53) (x finite, not 0) */
static void kern_split(double x, uint64_t* m, int* p) {
  const uint64_t u = kern_bits(x);
  int e = (int)((u >> 52) & 0x7ff);
  uint64_t f = u & 0x000fffffffffffffull;
  if (e == 0) {
    e = 1;
    while (!(f & 0x0010000000000000ull)) f <<= 1, --e;
  } else {
    f |= 0x0010000000000000ull;
  }
  *m = f;
  *p = e - 1075;
}

double kern_math_remainder(double x, double y) {
  if (!(x == x) || !(y == y)) return x + y;      /* NaN */
  const double a = kern_abs(x), b = kern_abs(y);
  if (a == __builtin_inf() || b == 0.0) return kern_musl___math_invalid(x);
  if (b == __builtin_inf()) return x;
  double r;
  int odd = 0;                                   /* (whether the whole quotient of a by b is odd) */
  if (a < b) {
    r = a;
  } else {
    uint64_t mx, my;
    int ex, ey;
    kern_split(a, &mx, &ex);
    kern_split(b, &my, &ey);
    /* long division, one bit of the quotient per step: mx * 2^ex mod my * 2^ey (a >= b, so ex >= ey) */
    for (; ex > ey; --ex) {
      if (mx >= my) mx -= my;
      mx <<= 1;
    }
    if (mx >= my) mx -= my, odd = 1;
    /* (the last step's bit is the quotient's lowest; earlier steps only matter for what is left) */
    r = mx == 0 ? 0.0 : kern_math_ldexp((double)mx, ey);   /* (exact: mx < 2^53, r < b is a double) */
  }
  /* r in [0, b): to the nearest multiple, a tie to the even one. r - b is exact (Sterbenz: r >= b / 2). */
  const int above = b >= 0x1p1022 ? (r > 0.5 * b ? 1 : r == 0.5 * b ? 0 : -1) : (2.0 * r > b ? 1 : 2.0 * r == b ? 0 : -1);
  if (above > 0 || (above == 0 && odd)) r = r - b;
  return __builtin_signbit(x) ? -r : r;          /* (r = 0: the sign of x) */
}

float kern_math_remainderf(float x, float y) {
  /* (the remainder of two floats is a float: exact in f64, the conversion changes nothing) */
  return (float)kern_math_remainder((double)x, (double)y);
}

double kern_math_log2(double x) {
  if (!(x == x)) return x + x;                   /* NaN */
  if (x < 0.0) return kern_musl___math_invalid(x);         /* (also -inf) */
  if (x == 0.0) return kern_musl___math_divzero(1);        /* -inf */
  if (x == __builtin_inf()) return x;
  uint64_t u = kern_bits(x);
  int k = 0;
  if (u < 0x0010000000000000ull) {               /* subnormal */
    x *= 0x1p54;
    k = -54;
    u = kern_bits(x);
  }
  k += (int)(u >> 52) - 1023;
  double m = kern_double((u & 0x000fffffffffffffull) | 0x3ff0000000000000ull);   /* [1, 2) */
  if (m > 0x1.6a09e667f3bcdp+0) m *= 0.5, ++k;   /* [sqrt(1/2), sqrt 2) */
  const double l = kern_musl_log(m);             /* |l| <= ln 2 / 2 */
  double ph, pl;
  kern_two_prod(l, 0x1.71547652b82fep+0, &ph, &pl);   /* l * (1/ln 2)_hi exactly */
  const double tail = pl + l * 0x1.777d0ffda0d24p-56; /* + l * (1/ln 2)_lo */
  if (k == 0) return ph + tail;
  const double kd = (double)k;
  const double s = kd + ph;                      /* |kd| >= 1 > |ph|: Fast2Sum */
  return s + (((kd - s) + ph) + tail);
}

float kern_math_log2f(float x) { return (float)kern_math_log2((double)x); }

double kern_math_log1p(double y) {
  if (!(y == y)) return y + y;                   /* NaN */
  if (y == __builtin_inf()) return y;
  if (y <= -1.0) return y == -1.0 ? kern_musl___math_divzero(1) : kern_musl___math_invalid(y);
  if (kern_abs(y) < 0x1p-29) {                   /* (also +-0): y - y^2/2 + y^3/3 - y^4/4 (the next term is below 2^-116 of y) - */
    return y + (y * y) * (-0.5 + y * (0x1.5555555555555p-2 - 0.25 * y));   /* (log u would lose to cancellation) */
  }
  const double u = 1.0 + y;
  /* y - (u - 1) is the exact error of 1 + y (Fast2Sum for |y| <= 1; for y > 1 u - 1 and y are within a factor of 2,
   * Sterbenz) - up to u = 2^53, beyond it c / u is below 2^-52 of log u */
  return kern_musl_log(u) + (y - (u - 1.0)) / u;
}

float kern_math_log1pf(float x) { return (float)kern_math_log1p((double)x); }

double kern_math_exp2(double x) {
  if (!(x == x)) return x + x;                   /* NaN */
  if (x >= 1024.0) return x == __builtin_inf() ? x : kern_math_ldexp(1.0, 1024);   /* inf */
  if (x < -1080.0) return 0.0;                   /* (also -inf: below half the smallest subnormal) */
  const double kd = (x + 0x1.8p52) - 0x1.8p52;   /* the whole number nearest x (|x| < 2^51) */
  const double f = x - kd;                       /* exact, |f| <= 1/2 */
  if (f == 0.0) return kern_math_ldexp(1.0, (int)kd);
  return kern_math_ldexp(kern_musl_exp(f * 0x1.62e42fefa39efp-1), (int)kd);
}

float kern_math_exp2f(float x) { return (float)kern_math_exp2((double)x); }

double kern_math_cbrt(double x) {
  const double a = kern_abs(x);
  if (!(a == a) || a == __builtin_inf() || a == 0.0) return x + x;   /* NaN, +-inf, +-0 (x + x keeps -0) */
  uint64_t u = kern_bits(a);
  int adjust = 0;
  if (u < 0x0010000000000000ull) {               /* subnormal: times 2^54, the root times 2^18 */
    u = kern_bits(a * 0x1p54);
    adjust = -18;
  }
  const int e = (int)(u >> 52) - 1023;
  const int q = e >= 0 ? e / 3 : -((-e + 2) / 3);   /* (floor) */
  const int r = e - 3 * q;                          /* 0, 1, 2 */
  const double m = kern_double((u & 0x000fffffffffffffull) | ((uint64_t)(1023 + r) << 52));   /* [1, 8) */
  const double y = kern_musl_exp(kern_musl_log(m) / 3.0);
  /* Newton: y - (y^3 - m) / (3 y^2), y^3 - m from exact products (y^2 = p + pe, p * y = c + ce; c - m exact: Sterbenz) */
  double p, pe, c, ce;
  kern_two_prod(y, y, &p, &pe);
  kern_two_prod(p, y, &c, &ce);
  const double residual = ((c - m) + ce) + pe * y;
  const double root = y - residual / (3.0 * p);
  return __builtin_copysign(kern_math_ldexp(root, q + adjust), x);
}

float kern_math_cbrtf(float x) { return (float)kern_math_cbrt((double)x); }

/* Under the names of the standard library, as musl_math/src/std_names.c gives musl's (the switches that keep a compiler
 * from computing them itself: musl_math/flags.sh). */
double ldexp(double x, int n) { return kern_math_ldexp(x, n); }
float ldexpf(float x, int n) { return kern_math_ldexpf(x, n); }
double remainder(double x, double y) { return kern_math_remainder(x, y); }
float remainderf(float x, float y) { return kern_math_remainderf(x, y); }
double log2(double x) { return kern_math_log2(x); }
float log2f(float x) { return kern_math_log2f(x); }
double log1p(double x) { return kern_math_log1p(x); }
float log1pf(float x) { return kern_math_log1pf(x); }
double exp2(double x) { return kern_math_exp2(x); }
float exp2f(float x) { return kern_math_exp2f(x); }
double cbrt(double x) { return kern_math_cbrt(x); }
float cbrtf(float x) { return kern_math_cbrtf(x); }
