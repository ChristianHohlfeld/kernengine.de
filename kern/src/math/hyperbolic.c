/* The engine's own hyperbolic functions: sinh, cosh, tanh, asinh (f64 and f32). Kern's code under Kern's licence - NOT
 * from musl (kern/third_party/musl_math has none of these). Written from the definitions
 *     sinh x = (e^x - e^-x) / 2      cosh x = (e^x + e^-x) / 2      tanh x = sinh x / cosh x      asinh x = ln(x + sqrt(x^2 + 1))
 * over what the engine already has - musl's exp, expm1 and log (kern_musl_*), sqrt (exact in IEEE) - with nothing but
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
 *         log1p(a + a^2 / (1 + sqrt(1 + a^2))), with log1p(y) = log(u) + (y - (u - 1)) / u, u = 1 + y (the error of the
 *         sum 1 + y put back) - the engine has no log1p of its own.
 * NaN gives NaN, +-0 gives +-0 (cosh: 1), +-inf gives +-inf (cosh: +inf, tanh: +-1). The f32 functions compute in f64 and
 * round once to f32. Accuracy: kern/tests/hyperbolic_reference.h (against values computed with 200 digits), the bound in
 * the test. */
double kern_musl_exp(double), kern_musl_expm1(double), kern_musl_log(double);

static double kern_abs(double x) { return __builtin_fabs(x); }
static double kern_with_sign(double v, double x) { return __builtin_copysign(v, x); }

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

/* log(1 + y) for y >= 0 small or large: the sum 1 + y rounded, its error put back (first order). */
static double kern_log1p(double y) {
  const double u = 1.0 + y;
  if (u == 1.0) return y;
  return kern_musl_log(u) + (y - (u - 1.0)) / u;
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
    r = kern_log1p(a + s / (1.0 + __builtin_sqrt(1.0 + s)));
  }
  return kern_with_sign(r, x);
}

float kern_math_sinhf(float x) { return (float)kern_math_sinh((double)x); }
float kern_math_coshf(float x) { return (float)kern_math_cosh((double)x); }
float kern_math_tanhf(float x) { return (float)kern_math_tanh((double)x); }
float kern_math_asinhf(float x) { return (float)kern_math_asinh((double)x); }

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
