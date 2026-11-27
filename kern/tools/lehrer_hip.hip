// Native-only, opt-in. The sums use the CPU's order; no fused operations.
// Physics, small-batch inference, ELU, Adam and PPO control stay on CPU.
#include <hip/hip_runtime.h>
#include "kern/lehrer/hip_dense.h"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mutex>
#include <vector>
namespace kern::lehrer {
namespace {
void check(hipError_t e) {
  if (e != hipSuccess) {
    std::fprintf(stderr, "Lehrer HIP: %s\n", hipGetErrorString(e));
    std::abort();  // no silent fallback to CPU
  }
}
struct Buffer {
  f32* p = nullptr;
  size_t capacity = 0;
  void need(size_t n) {
    if (n <= capacity) return;
    if (p) check(hipFree(p));
    check(hipMalloc(reinterpret_cast<void**>(&p), n * sizeof(f32)));
    capacity = n;
  }
  void put(const f32* src, size_t n) {
    need(n); check(hipMemcpy(p, src, n * sizeof(f32), hipMemcpyHostToDevice));
  }
  void get(f32* dst, size_t n) {
    check(hipMemcpy(dst, p, n * sizeof(f32), hipMemcpyDeviceToHost));
  }
};
struct Device {
  std::mutex mutex;
  Buffer x, w, b, y;
};
struct Devices {
  std::vector<std::unique_ptr<Device>> all;
  std::atomic<unsigned> next{0};
  Devices() {
    int n = 0; check(hipGetDeviceCount(&n));
    if (n < 1) { std::fprintf(stderr, "Lehrer HIP: no GPU\n"); std::abort(); }
    for (int i = 0; i < n; ++i) all.push_back(std::make_unique<Device>());
  }
};
Devices& devices() { static Devices d; return d; }
int pick() {
  auto& d = devices();
  return static_cast<int>(d.next.fetch_add(1, std::memory_order_relaxed) % d.all.size());
}
__global__ void dense_kernel(const f32* x, int rows, int in, int out, const f32* w, const f32* b, f32* y) {
  const size_t k = size_t(blockIdx.x) * blockDim.x + threadIdx.x;
  if (k >= size_t(rows) * out) return;
  const int o = int(k % out), r = int(k / out);
  f32 s = b[o];
  for (int i = 0; i < in; ++i) s += w[size_t(i) * out + o] * x[size_t(r) * in + i];
  y[k] = s;
}
__global__ void gradient_kernel(const f32* x, const f32* d, int rows, int in, int out, f32* gw, f32* gb) {
  const size_t k = size_t(blockIdx.x) * blockDim.x + threadIdx.x;
  if (k >= size_t(in) * out + out) return;
  if (k >= size_t(in) * out) {
    int o = int(k - size_t(in) * out);
    f32 s = gb[o];
    for (int r = 0; r < rows; ++r) s += d[size_t(r) * out + o];
    gb[o] = s; return;
  }
  const int i = int(k / out), o = int(k % out);
  f32 s = gw[k];
  int r = 0;
  for (; r + 4 <= rows; r += 4) {
    const size_t a = size_t(r) * in + i, z = size_t(r) * out + o;
    s += x[a] * d[z] + x[a+in] * d[z+out] + x[a+2*in] * d[z+2*out] + x[a+3*in] * d[z+3*out];
  }
  for (; r < rows; ++r) s += x[size_t(r) * in + i] * d[size_t(r) * out + o];
  gw[k] = s;
}
void dense_on(int id, const f32* x, int rows, int in, int out, const f32* w, const f32* b, f32* y) {
  Device& d = *devices().all[id]; std::lock_guard<std::mutex> lock(d.mutex);
  check(hipSetDevice(id));
  d.x.put(x, size_t(rows)*in); d.w.put(w, size_t(in)*out); d.b.put(b,out); d.y.need(size_t(rows)*out);
  hipLaunchKernelGGL(dense_kernel, dim3((size_t(rows)*out+255)/256), dim3(256), 0, 0, d.x.p, rows, in, out, d.w.p, d.b.p, d.y.p);
  check(hipGetLastError()); d.y.get(y,size_t(rows)*out);
}
void gradient_on(int id, const f32* x, const f32* delta, int rows, int in, int out, f32* gw, f32* gb) {
  Device& d = *devices().all[id]; std::lock_guard<std::mutex> lock(d.mutex);
  check(hipSetDevice(id));
  d.x.put(x,size_t(rows)*in); d.y.put(delta,size_t(rows)*out);
  d.w.put(gw,size_t(in)*out); d.b.put(gb,out);
  hipLaunchKernelGGL(gradient_kernel, dim3((size_t(in)*out+out+255)/256), dim3(256), 0, 0, d.x.p, d.y.p, rows, in, out, d.w.p, d.b.p);
  check(hipGetLastError()); d.w.get(gw,size_t(in)*out); d.b.get(gb,out);
}
}
int hip_lehrer_devices() { return static_cast<int>(devices().all.size()); }
void hip_lehrer_dense(const f32* x, i32 rows, i32 in, i32 out, const f32* w, const f32* b, f32* y) {
  dense_on(pick(),x,rows,in,out,w,b,y);
}
void hip_lehrer_gradient(const f32* x, const f32* d, i32 rows, i32 in, i32 out, f32* gw, f32* gb) {
  gradient_on(pick(),x,d,rows,in,out,gw,gb);
}
bool hip_lehrer_check() {
  constexpr int rows=19, in=23, out=31;
  std::vector<f32> x(rows*in), w(in*out), b(out), d(rows*out), y(rows*out), ref(y.size());
  unsigned seed=17;
  auto fill=[&](auto& a){ for(auto& v:a) { seed=seed*1664525u+1013904223u; v=float(int(seed%20001)-10000)/12345.0f; } };
  fill(x);fill(w);fill(b);fill(d);
  for(int r=0;r<rows;++r) for(int o=0;o<out;++o) {
    f32 s=b[o]; for(int i=0;i<in;++i) s+=w[i*out+o]*x[r*in+i]; ref[r*out+o]=s;
  }
  auto eq=[](const auto& a,const auto& b){return std::memcmp(a.data(),b.data(),a.size()*sizeof(f32))==0;};
  auto rg=w, rb=b;
  for(int o=0;o<out;++o) for(int r=0;r<rows;++r) rb[o]+=d[r*out+o];
  for(int i=0;i<in;++i) for(int o=0;o<out;++o){
    f32 s=rg[i*out+o]; int r=0;
    for(;r+4<=rows;r+=4) s+=x[r*in+i]*d[r*out+o]+x[(r+1)*in+i]*d[(r+1)*out+o]+x[(r+2)*in+i]*d[(r+2)*out+o]+x[(r+3)*in+i]*d[(r+3)*out+o];
    for(;r<rows;++r) s+=x[r*in+i]*d[r*out+o];
    rg[i*out+o]=s;
  }
  for(int id=0;id<hip_lehrer_devices();++id){
    dense_on(id,x.data(),rows,in,out,w.data(),b.data(),y.data());
    auto gw=w,gb=b; gradient_on(id,x.data(),d.data(),rows,in,out,gw.data(),gb.data());
    if(!eq(y,ref)||!eq(gw,rg)||!eq(gb,rb)){std::fprintf(stderr,"Lehrer HIP: CPU/GPU check failed on device %d\n",id);return false;}
  }
  return true;
}
}
