/* The engine's own hyperbolic functions: sinh, cosh, tanh, asinh, acosh, atanh (f64 and f32). Kern's code under Kern's licence - NOT
 * from musl (kern/third_party/musl_math has none of these). Written from the definitions
 *     sinh x = (e^x - e^-x) / 2      cosh x = (e^x + e^-x) / 2      tanh x = sinh x / cosh x      asinh x = ln(x + sqrt(x^2 + 1))
 *     acosh x = ln(x + sqrt(x^2 - 1))   atanh x = ln((1 + x) / (1 - x)) / 2
 * over what the engine already has - musl's exp, expm1 and log (kern_musl_*), sqrt (exact in IEEE), the engine's log1p
 * (elementary.c) - with nothing but
 * + - * / and those: the same bits on every target (x86, ARM, WebAssembly), built like musl_math (-ffp-contract=off).
 * Every build takes this file where it takes musl_math (kern/docs/blueprint_cpp.md "Mathe-Funktionen"); kern::dm, the
 * Blueprint's sinh/cosh/tanh/asinh and the standard names below (std::sinh ...) all call these.
 *
 * The forms (a = |x|; the sign is put back at the end - all four but cosh are odd):
 *   sinh: a < 2^-28: x (x^3/6 is below half an ulp). Else t = expm1(a), sinh a = (t + t/(t+1)) / 2 - e^a - e^-a without
 *         the cancellation near 0. a > 709 (e^a would overflow first): h = exp(a/2), (h/2) * h.
 *   cosh: a < 2^-27: 1. a < ln2/2: t = expm1(a), 1 + t^2 / (2(1+t)) (cosh a - 1 = (e^a - 1)^2 / (2 e^a)). a < 22: e = exp(a),
 *         e/2 + 0.5/e. a < 709: exp(a)/2. Above: (h/2) * h as sinh (inf where it overflows). NaN: NaN, +-inf: +inf.
 *   tanh: a < 2^-28: x. a > 22: 1 (1 - tanh a = 2/(e^2a + 1) < 2^-63). a > 0.55: t = expm1(2a), 1 - 2/(t + 2). Else
 *         t = expm1(-2a), -t / (t + 2).
 *   asinh: a < 2^-28: x. a > 2^28: log(a) + ln 2 (sqrt(a^2 + 1) is a). a > 2: log(2a + 1/(sqrt(a^2+1) + a)). Else
 *         log1p(a + a^2 / (1 + sqrt(1 + a^2))) (the engine's log1p, elementary.c).
 *   acosh: x < 1: NaN. x > 2^28: log(x) + ln 2. x > 2: log(2x - 1/(x + sqrt(x^2 - 1))). Else t = x - 1 (exact),
 *         log1p(t + sqrt(2t + t^2)) with the errors of the root and the sum kept and put back (Dekker, Fast2Sum).
 *         acosh 1 = +0, acosh inf = inf.
 *   atanh: a > 1: NaN, a = 1: +-inf. a < 2^-28: x. a < 1/2: log1p(2a + 2a^2 / (1 - a)) / 2. Else log1p(2a / (1 - a)) / 2
 *         (1 - a exact). Odd.
 * NaN gives NaN, +-0 gives +-0 (cosh: 1), +-inf gives +-inf (cosh: +inf, tanh: +-1). The f32 functions compute in f64 and
 * round once to f32. Accuracy: kern/tests/hyperbolic_reference.h, for acosh and atanh kern/tests/elementary_reference.h
 * (against values computed with 200 digits), the bound in the tests. */
#include <stdint.h>

double kern_musl_exp(double), kern_musl_expm1(double), kern_musl_log(double), kern_math_log1p(double);
double kern_musl___math_invalid(double);    /* (musl_math: NaN the way musl's log makes it) */
double kern_musl___math_divzero(uint32_t);  /* (musl_math: +-inf the way musl's log makes it) */

static double kern_abs(double x) { return __builtin_fabs(x); }
static double kern_with_sign(double v, double x) { return __builtin_copysign(v, x); }

/* a * b = hi + lo exactly (no overflow): Veltkamp's split, Dekker's product - + - * only (as elementary.c). */
static void kern_two_prod(double a, double b, double* hi, double* lo) {
  const double c = 134217729.0;   /* 2^27 + 1 */
  double t = c * a;
  const double ah = t - (t - a), al = a - ah;
  t = c * b;
  const double bh = t - (t - b), bl = b - bh;
  *hi = a * b;
  *lo = ((ah * bh - *hi) + ah * bl + al * bh) + al * bl;
}

double kern_math_sinh(double x) {
  const double a = kern_abs(x);
  if (!(a == a)) return x + x;                   /* NaN */
  if (a < 0x1p-28) return x;                     /* (also +-0) */
  if (a > 709.0) {                               /* (also +-inf: inf) */
    const double h = kern_musl_exp(0.5 * a);
    return kern_with_sign((0.5 * h) * h, x);
  }
  const double t = kern_musl_expm1(a);
  return kern_with_sign(0.5 * (t + t / (t + 1.0)), x);
}

double kern_math_cosh(double x) {
  const double a = kern_abs(x);
  if (!(a == a)) return x * x;                   /* NaN */
  if (a < 0x1p-27) return 1.0;
  if (a < 0.34657359027997264) {                 /* (ln 2 / 2) */
    const double t = kern_musl_expm1(a);
    return 1.0 + (t * t) / (2.0 * (1.0 + t));
  }
  if (a < 22.0) {
    const double e = kern_musl_exp(a);
    return 0.5 * e + 0.5 / e;
  }
  if (a < 709.0) return 0.5 * kern_musl_exp(a);
  const double h = kern_musl_exp(0.5 * a);       /* (also inf) */
  return (0.5 * h) * h;
}

double kern_math_tanh(double x) {
  const double a = kern_abs(x);
  if (!(a == a)) return x + x;                   /* NaN */
  if (a < 0x1p-28) return x;                     /* (also +-0) */
  if (a > 22.0) return kern_with_sign(1.0, x);   /* (also +-inf) */
  double r;
  if (a > 0.55) {
    const double t = kern_musl_expm1(2.0 * a);
    r = 1.0 - 2.0 / (t + 2.0);
  } else {
    const double t = kern_musl_expm1(-2.0 * a);
    r = -t / (t + 2.0);
  }
  return kern_with_sign(r, x);
}

double kern_math_asinh(double x) {
  const double a = kern_abs(x);
  if (!(a == a) || a == __builtin_inf()) return x + x;   /* NaN, +-inf */
  if (a < 0x1p-28) return x;                     /* (also +-0) */
  double r;
  if (a > 0x1p28) {
    r = kern_musl_log(a) + 0.6931471805599453094;   /* (ln 2) */
  } else if (a > 2.0) {
    r = kern_musl_log(2.0 * a + 1.0 / (__builtin_sqrt(a * a + 1.0) + a));
  } else {
    const double s = a * a;
    r = kern_math_log1p(a + s / (1.0 + __builtin_sqrt(1.0 + s)));
  }
  return kern_with_sign(r, x);
}

double kern_math_acosh(double x) {
  if (!(x == x)) return x + x;                   /* NaN */
  if (x < 1.0) return kern_musl___math_invalid(x);         /* (also -inf) */
  if (x == __builtin_inf()) return x;
  if (x > 0x1p28) return kern_musl_log(x) + 0.6931471805599453094;   /* (ln 2) */
  if (x > 2.0) return kern_musl_log(2.0 * x - 1.0 / (x + __builtin_sqrt(x * x - 1.0)));
  /* log1p(t + sqrt(2t + t^2)), t = x - 1 (exact: Sterbenz), each step with its error kept (Dekker's product, Fast2Sum,
   * a Newton step for the root), the error of the sum put back after log1p: near 1 the root's and the sum's rounding
   * would otherwise come on top of log1p's. */
  const double t = x - 1.0;
  if (t == 0.0) return 0.0;                      /* acosh 1 = +0 (the root of 0 has no Newton step) */
  double p, pe, q, qe;
  kern_two_prod(t, t, &p, &pe);
  const double sh = 2.0 * t + p, sl = ((2.0 * t - sh) + p) + pe;   /* 2t + t^2 = sh + sl (2t >= t^2) */
  const double s = __builtin_sqrt(sh);
  kern_two_prod(s, s, &q, &qe);
  const double sc = (((sh - q) - qe) + sl) / (2.0 * s);              /* sqrt(sh + sl) = s + sc */
  const double z = s + t, zl = ((s - z) + t) + sc;                   /* s + t + sc = z + zl (s >= t) */
  return kern_math_log1p(z) + zl / (1.0 + z);
}

double kern_math_atanh(double x) {
  const double a = kern_abs(x);
  if (!(a == a)) return x + x;                   /* NaN */
  if (a > 1.0) return kern_musl___math_invalid(x);
  if (a == 1.0) return kern_musl___math_divzero(x < 0.0 ? 1 : 0);   /* +-inf */
  if (a < 0x1p-28) return x;                     /* (also +-0) */
  const double d = 1.0 - a;
  const double r = a < 0.5 ? 0.5 * kern_math_log1p(2.0 * a + 2.0 * a * a / d) : 0.5 * kern_math_log1p(2.0 * a / d);
  return kern_with_sign(r, x);
}

float kern_math_sinhf(float x) { return (float)kern_math_sinh((double)x); }
float kern_math_coshf(float x) { return (float)kern_math_cosh((double)x); }
float kern_math_tanhf(float x) { return (float)kern_math_tanh((double)x); }
float kern_math_asinhf(float x) { return (float)kern_math_asinh((double)x); }
float kern_math_acoshf(float x) { return (float)kern_math_acosh((double)x); }
float kern_math_atanhf(float x) { return (float)kern_math_atanh((double)x); }

/* Under the names of the standard library, as musl_math/src/std_names.c gives musl's (the switches that keep a compiler
 * from computing them itself: musl_math/flags.sh). */
double sinh(double x) { return kern_math_sinh(x); }
double cosh(double x) { return kern_math_cosh(x); }
double tanh(double x) { return kern_math_tanh(x); }
double asinh(double x) { return kern_math_asinh(x); }
float sinhf(float x) { return kern_math_sinhf(x); }
float coshf(float x) { return kern_math_coshf(x); }
float tanhf(float x) { return kern_math_tanhf(x); }
float asinhf(float x) { return kern_math_asinhf(x); }
double acosh(double x) { return kern_math_acosh(x); }
double atanh(double x) { return kern_math_atanh(x); }
float acoshf(float x) { return kern_math_acoshf(x); }
float atanhf(float x) { return kern_math_atanhf(x); }
