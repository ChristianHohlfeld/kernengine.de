/* The engine's musl under the names of the standard library (kern/docs/blueprint_cpp.md "Mathe-Funktionen"): a program's
 * std::sin, sinf, atan2 ... - its own C++, the firmware it holds, a library it builds - is the function kern::dm and the
 * Blueprint's nodes call, on every target (natively these definitions come before the system's libm, in WebAssembly before
 * wasi-libc's). Linked wherever musl_math is (every build takes src/*.c); the switches that keep the compiler from computing
 * these functions itself are in ../flags.sh. sincos/sincosf: what a compiler makes of sin and cos of one value. */
float kern_musl_sinf(float), kern_musl_cosf(float), kern_musl_tanf(float), kern_musl_asinf(float), kern_musl_acosf(float),
      kern_musl_atanf(float), kern_musl_atan2f(float, float), kern_musl_expf(float), kern_musl_expm1f(float), kern_musl_logf(float),
      kern_musl_log10f(float), kern_musl_powf(float, float), kern_musl_hypotf(float, float);
double kern_musl_sin(double), kern_musl_cos(double), kern_musl_tan(double), kern_musl_asin(double), kern_musl_acos(double),
       kern_musl_atan(double), kern_musl_atan2(double, double), kern_musl_exp(double), kern_musl_expm1(double), kern_musl_log(double),
       kern_musl_log10(double), kern_musl_pow(double, double), kern_musl_hypot(double, double);

float sinf(float x) { return kern_musl_sinf(x); }
float cosf(float x) { return kern_musl_cosf(x); }
float tanf(float x) { return kern_musl_tanf(x); }
float asinf(float x) { return kern_musl_asinf(x); }
float acosf(float x) { return kern_musl_acosf(x); }
float atanf(float x) { return kern_musl_atanf(x); }
float atan2f(float y, float x) { return kern_musl_atan2f(y, x); }
float expf(float x) { return kern_musl_expf(x); }
float expm1f(float x) { return kern_musl_expm1f(x); }
float logf(float x) { return kern_musl_logf(x); }
float log10f(float x) { return kern_musl_log10f(x); }
float powf(float x, float y) { return kern_musl_powf(x, y); }
float hypotf(float x, float y) { return kern_musl_hypotf(x, y); }
void sincosf(float x, float* s, float* c) { *s = kern_musl_sinf(x); *c = kern_musl_cosf(x); }
double sin(double x) { return kern_musl_sin(x); }
double cos(double x) { return kern_musl_cos(x); }
double tan(double x) { return kern_musl_tan(x); }
double asin(double x) { return kern_musl_asin(x); }
double acos(double x) { return kern_musl_acos(x); }
double atan(double x) { return kern_musl_atan(x); }
double atan2(double y, double x) { return kern_musl_atan2(y, x); }
double exp(double x) { return kern_musl_exp(x); }
double expm1(double x) { return kern_musl_expm1(x); }
double log(double x) { return kern_musl_log(x); }
double log10(double x) { return kern_musl_log10(x); }
double pow(double x, double y) { return kern_musl_pow(x, y); }
double hypot(double x, double y) { return kern_musl_hypot(x, y); }
void sincos(double x, double* s, double* c) { *s = kern_musl_sin(x); *c = kern_musl_cos(x); }
