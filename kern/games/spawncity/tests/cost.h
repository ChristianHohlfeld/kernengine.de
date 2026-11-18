// What a function's own work costs: how often it asks for memory.
//
// The allocator of the tests counts while `counting` is on (test_main.cpp). The comparison of a
// converted function with its source turns it on around the source's call and - through the
// engine's logic::native_probe - around the call of the Blueprint's compiled class (converted.h):
// the class does not ask for memory more often than the source it was written from.
//
// Where it asks: SC_COST_WHERE=<function> (the name a test's cost line has) keeps, for that function,
// from where each piece of memory is asked for - in the source and in the class -, and the first
// call in which the class asks more often prints both (converted.h, Tally::take). A tool to find
// out why; it changes nothing that is counted.
#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#if defined(__SANITIZE_ADDRESS__)
#include <execinfo.h>
extern "C" void __sanitizer_symbolize_pc(void* pc, const char* format, char* out, unsigned long size);
#endif

namespace sc::cost {

inline bool counting = false;  // the allocator counts
inline long asks = 0;          // what it counted since the count began

/// From where memory was asked for during one count: the first kAsks of them, kDeep frames each.
struct Trace {
  static constexpr int kAsks = 24, kDeep = 20;
  void* frame[kAsks][kDeep];
  int deep[kAsks];
  int kept = 0;
};
inline bool tracing = false;   // the function being compared is the one SC_COST_WHERE names
inline Trace now;              // the count that runs, or the one that ended last

/// Called by the allocator for every piece of memory it is asked for.
inline void asked() {
  if (!counting) return;
  ++asks;
#if defined(__SANITIZE_ADDRESS__)
  if (tracing && now.kept < Trace::kAsks) {
    counting = false;  // (what taking the trace asks for is not the function's)
    now.deep[now.kept] = backtrace(now.frame[now.kept], Trace::kDeep);
    ++now.kept;
    counting = true;
  }
#endif
}
inline void begin() {
  asks = 0;
  now.kept = 0;
  counting = true;
}
inline long end() {
  counting = false;
  return asks;
}
/// The function SC_COST_WHERE names (nullptr: none).
inline const char* where() {
  static const char* const name = std::getenv("SC_COST_WHERE");
  return name;
}
/// Prints a trace: per piece of memory the functions it was asked for from, innermost first -
/// without the allocator's own frames and what lies outside the function's call.
inline void print(const char* who, const Trace& t, long asked_for) {
  std::printf("    %s asks %ld time(s)%s\n", who, asked_for, asked_for > t.kept ? " (the first are shown)" : "");
#if defined(__SANITIZE_ADDRESS__)
  for (int k = 0; k < t.kept; ++k) {
    std::printf("      %d:", k + 1);
    int shown = 0;
    for (int d = 0; d < t.deep[k] && shown < 7; ++d) {
      char text[512];
      __sanitizer_symbolize_pc(t.frame[k][d], "%f %S", text, sizeof text);
      // (the allocator and the count themselves, and from the comparison outward: not the function's)
      if (std::strstr(text, "sc::cost::") || std::strstr(text, "asked_for_memory") || std::strstr(text, "__sanitizer") || std::strstr(text, "__interceptor") ||
          std::strstr(text, "operator new") || std::strstr(text, "__asan") || std::strstr(text, "backtrace"))
        continue;
      if (std::strstr(text, "sc::converted::counted") || std::strstr(text, "Native::call") || std::strstr(text, "_Native")) break;
      std::printf("%s %.200s\n", shown ? "         <-" : "", text);
      ++shown;
    }
    if (!shown) std::printf(" (no frame of the function)\n");
  }
#endif
}

}  // namespace sc::cost
