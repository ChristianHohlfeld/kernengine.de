// Kleinstfall E9: launch(at, {size: rng, rng}, yaw, tint()) – old (arguments with draws) vs. new (named from the left)
#include <cstdio>
#include <cstdint>
struct Rng { uint32_t s = 12345; float unit() { s = s * 1664525u + 1013904223u; return (s >> 8) * (1.0f / 16777216.0f); }
             float range(float a, float b) { return a + (b - a) * unit(); } };
struct V { float x, y, z; };
static V got[2][3]; static int n;
static void launch(V at, V size, float yaw, V tint, int k) { got[k][0] = size; got[k][1] = tint; got[k][2] = {at.x, yaw, 0}; }
int main() {
  for (int k = 0; k < 2; ++k) {
    Rng rng; auto tint = [&](float e) { const float u = rng.unit(); return u < e ? V{1, 0, 0} : V{0, 1, 0}; };
    if (k == 0) launch({rng.range(0, 1), 0, 0}, {rng.range(3, 7), rng.range(3, 7), 0.5f}, rng.unit() * 6.28f, tint(0.25f), 0);
    else { const V at{rng.range(0, 1), 0, 0}; const V size{rng.range(3, 7), rng.range(3, 7), 0.5f}; const float yaw = rng.unit() * 6.28f; const V col = tint(0.25f); launch(at, size, yaw, col, 1); }
  }
  bool same = true;
  for (int i = 0; i < 3; ++i) same = same && got[0][i].x == got[1][i].x && got[0][i].y == got[1][i].y && got[0][i].z == got[1][i].z;
  std::printf("old size.x %.9g new size.x %.9g -> %s\n", got[0][0].x, got[1][0].x, same ? "GLEICH" : "VERSCHIEDEN");
  return same ? 0 : 1;
}
