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
#include <thread>
#include <vector>
namespace kern::lehrer {
namespace {
void check(hipError_t e) {
  if (e != hipSuccess) {
    std::fprintf(stderr, "Lehrer HIP: %s\n", hipGetErrorString(e));
    std::abort();  // no silent fallback to CPU
  }
}
// Each lane owns its device and pinned host staging buffers. No buffer is
// reused until its stream has completed. Allocations grow on demand, outside
// the queued transfers; process-lifetime workspaces are reclaimed on exit.
struct Buffer {
  f32* p = nullptr;
  f32* host = nullptr;
  size_t capacity = 0;
  void need(size_t n) {
    if (n <= capacity) return;
    if (p) { check(hipFree(p)); check(hipHostFree(host)); }
    check(hipMalloc(reinterpret_cast<void**>(&p), n * sizeof(f32)));
    check(hipHostMalloc(reinterpret_cast<void**>(&host), n * sizeof(f32), hipHostMallocDefault));
    capacity = n;
  }
  void put(const f32* src, size_t n, hipStream_t stream) {
    std::memcpy(host, src, n * sizeof(f32));
    check(hipMemcpyAsync(p, host, n * sizeof(f32), hipMemcpyHostToDevice, stream));
  }
  void read(size_t n, hipStream_t stream) {
    check(hipMemcpyAsync(host, p, n * sizeof(f32), hipMemcpyDeviceToHost, stream));
  }
  void copy_out(f32* dst, size_t n) const {
    std::memcpy(dst, host, n * sizeof(f32));
  }
};
struct Lane {
  std::mutex mutex;
  hipStream_t stream = nullptr;
  Buffer x, w, b, y;
  void prepare(int id) {
    check(hipSetDevice(id));
    if (!stream) check(hipStreamCreateWithFlags(&stream, hipStreamNonBlocking));
  }
};
struct Device {
  std::vector<std::unique_ptr<Lane>> lanes;
};
int lane_count() {
  const char* value = std::getenv("KERN_HIP_STREAMS");
  if (!value) return 4;
  char* end = nullptr;
  const long n = std::strtol(value, &end, 10);
  if (end == value || *end || n < 1 || n > 8) {
    std::fprintf(stderr, "Lehrer HIP: KERN_HIP_STREAMS must be 1..8\n"); std::abort();
  }
  return static_cast<int>(n);
}
struct Devices {
  std::vector<std::unique_ptr<Device>> all;
  std::atomic<unsigned> next{0};
  const int lanes = lane_count();
  Devices() {
    int n = 0; check(hipGetDeviceCount(&n));
    if (n < 1) { std::fprintf(stderr, "Lehrer HIP: no GPU\n"); std::abort(); }
    for (int i = 0; i < n; ++i) {
      auto device = std::make_unique<Device>();
      for (int j = 0; j < lanes; ++j) device->lanes.push_back(std::make_unique<Lane>());
      all.push_back(std::move(device));
    }
  }
};
Devices& devices() { static Devices d; return d; }
struct Target { int id, lane; };
Target pick() {
  auto& d = devices();
  const unsigned n = d.next.fetch_add(1, std::memory_order_relaxed);
  // Distribute across devices first, then independent streams on each device.
  return {static_cast<int>(n % d.all.size()),
          static_cast<int>((n / d.all.size()) % static_cast<unsigned>(d.lanes))};
}
// 32 output columns x 8 observations per workgroup. Reuse weights
// and observations from LDS while keeping each output's original sum order.
__global__ void dense_kernel(const f32* x, int rows, int in, int out, const f32* w, const f32* b, f32* y) {
  constexpr int OC = 32, RC = 8, IC = 16;
  __shared__ f32 weights[IC][OC];
  __shared__ f32 inputs[RC][IC];
  const int t = int(threadIdx.x), c = t % OC, a = t / OC;
  const int o = int(blockIdx.x)*OC+c, r = int(blockIdx.y)*RC+a;
  f32 sum = o < out ? b[o] : 0.0f;
  for (int base = 0; base < in; base += IC) {
    for (int k = t; k < IC*OC; k += OC*RC) {
      const int i = base+k/OC, col = int(blockIdx.x)*OC+k%OC;
      weights[k/OC][k%OC] = i < in && col < out ? w[size_t(i)*out+col] : 0.0f;
    }
    if (t < RC*IC) {
      const int row = int(blockIdx.y)*RC+t/IC, i = base+t%IC;
      inputs[t/IC][t%IC] = row < rows && i < in ? x[size_t(row)*in+i] : 0.0f;
    }
    __syncthreads();
    const int count = in-base < IC ? in-base : IC;
    if (r < rows && o < out)
      for (int i=0; i<count; ++i) sum += weights[i][c]*inputs[a][i];
    __syncthreads();
  }
  if (r < rows && o < out) y[size_t(r)*out+o] = sum;
}
// Each weight still reduces the rows in exactly the original order (four
// products per addition), but neighboring weights share input/delta tiles.
__global__ void gradient_kernel(const f32* x, const f32* d, int rows, int in, int out, f32* gw) {
  constexpr int OC=32, IC=8, RC=16;
  __shared__ f32 inputs[RC][IC];
  __shared__ f32 deltas[RC][OC];
  const int t=int(threadIdx.x), c=t%OC, a=t/OC;
  const int i=int(blockIdx.y)*IC+a, o=int(blockIdx.x)*OC+c;
  const bool valid=i<in && o<out;
  f32 sum=valid ? gw[size_t(i)*out+o] : 0.0f;
  for(int base=0;base<rows;base+=RC){
    if(t<RC*IC){
      const int row=base+t/IC, col=int(blockIdx.y)*IC+t%IC;
      inputs[t/IC][t%IC]=row<rows && col<in ? x[size_t(row)*in+col] : 0.0f;
    }
    for(int k=t;k<RC*OC;k+=IC*OC){
      const int row=base+k/OC, col=int(blockIdx.x)*OC+k%OC;
      deltas[k/OC][k%OC]=row<rows && col<out ? d[size_t(row)*out+col] : 0.0f;
    }
    __syncthreads();
    const int count=rows-base<RC ? rows-base : RC;
    if(valid){
      int r=0;
      for(;r+4<=count;r+=4)
        sum += inputs[r][a]*deltas[r][c]+inputs[r+1][a]*deltas[r+1][c]+inputs[r+2][a]*deltas[r+2][c]+inputs[r+3][a]*deltas[r+3][c];
      for(;r<count;++r) sum += inputs[r][a]*deltas[r][c];
    }
    __syncthreads();
  }
  if(valid) gw[size_t(i)*out+o]=sum;
}
__global__ void bias_gradient_kernel(const f32* d, int rows, int out, f32* gb) {
  const int o=int(blockIdx.x)*int(blockDim.x)+int(threadIdx.x);
  if(o>=out) return;
  f32 sum=gb[o];
  for(int r=0;r<rows;++r) sum+=d[size_t(r)*out+o];
  gb[o]=sum;
}
void dense_on(Target target, const f32* x, int rows, int in, int out, const f32* w, const f32* b, f32* y) {
  Lane& d = *devices().all[target.id]->lanes[target.lane];
  std::lock_guard<std::mutex> lock(d.mutex);
  d.prepare(target.id);
  const size_t nx = size_t(rows)*in, nw = size_t(in)*out, ny = size_t(rows)*out;
  d.x.need(nx); d.w.need(nw); d.b.need(out); d.y.need(ny);
  d.x.put(x,nx,d.stream); d.w.put(w,nw,d.stream); d.b.put(b,out,d.stream);
  hipLaunchKernelGGL(dense_kernel, dim3((out+31)/32, (rows+7)/8), dim3(256), 0, d.stream, d.x.p, rows, in, out, d.w.p, d.b.p, d.y.p);
  check(hipGetLastError());
  d.y.read(ny,d.stream);
  check(hipStreamSynchronize(d.stream));
  d.y.copy_out(y,ny);
}
void gradient_on(Target target, const f32* x, const f32* delta, int rows, int in, int out, f32* gw, f32* gb) {
  Lane& d = *devices().all[target.id]->lanes[target.lane];
  std::lock_guard<std::mutex> lock(d.mutex);
  d.prepare(target.id);
  const size_t nx = size_t(rows)*in, nw = size_t(in)*out, ny = size_t(rows)*out;
  d.x.need(nx); d.y.need(ny); d.w.need(nw); d.b.need(out);
  d.x.put(x,nx,d.stream); d.y.put(delta,ny,d.stream);
  d.w.put(gw,nw,d.stream); d.b.put(gb,out,d.stream);
  hipLaunchKernelGGL(gradient_kernel, dim3((out+31)/32, (in+7)/8), dim3(256), 0, d.stream, d.x.p, d.y.p, rows, in, out, d.w.p);
  check(hipGetLastError());
  hipLaunchKernelGGL(bias_gradient_kernel, dim3((out+255)/256), dim3(256), 0, d.stream, d.y.p, rows, out, d.b.p);
  check(hipGetLastError());
  d.w.read(nw,d.stream); d.b.read(out,d.stream);
  check(hipStreamSynchronize(d.stream));
  d.w.copy_out(gw,nw); d.b.copy_out(gb,out);
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
  std::vector<f32> x(rows*in), w(in*out), b(out), d(rows*out);
  unsigned seed=17;
  auto fill=[&](auto& a){ for(auto& v:a) { seed=seed*1664525u+1013904223u; v=float(int(seed%20001)-10000)/12345.0f; } };
  fill(x);fill(w);fill(b);fill(d);
  auto eq=[](const auto& a,const auto& b){return std::memcmp(a.data(),b.data(),a.size()*sizeof(f32))==0;};
  // Exercise every lane concurrently, with independent outputs and inputs.
  // Distinct per-lane initial biases/gradients catch cross-stream contamination.
  std::atomic<bool> ok{true};
  std::vector<std::thread> pool;
  const int ngpu = hip_lehrer_devices(), lanes = devices().lanes;
  for (int id=0;id<ngpu;++id) for(int lane=0;lane<lanes;++lane) {
    pool.emplace_back([&,id,lane] {
      auto local_b=b, local_w=w, local_d=d;
      const float offset = float(id*lanes+lane)*0.125f;
      for(auto& v:local_b) v+=offset;
      for(auto& v:local_w) v-=offset;
      for(auto& v:local_d) v+=offset;
      auto local_rg=local_w, local_rb=local_b;
      std::vector<f32> local_y(rows*out), local_ref(rows*out);
      for(int r=0;r<rows;++r) for(int o=0;o<out;++o){
        f32 sum=local_b[o]; for(int i=0;i<in;++i) sum+=local_w[i*out+o]*x[r*in+i]; local_ref[r*out+o]=sum;
      }
      for(int o=0;o<out;++o) for(int r=0;r<rows;++r) local_rb[o]+=local_d[r*out+o];
      for(int i=0;i<in;++i) for(int o=0;o<out;++o){
        f32 sum=local_rg[i*out+o]; int r=0;
        for(;r+4<=rows;r+=4) sum+=x[r*in+i]*local_d[r*out+o]+x[(r+1)*in+i]*local_d[(r+1)*out+o]+x[(r+2)*in+i]*local_d[(r+2)*out+o]+x[(r+3)*in+i]*local_d[(r+3)*out+o];
        for(;r<rows;++r) sum+=x[r*in+i]*local_d[r*out+o];
        local_rg[i*out+o]=sum;
      }
      const Target target{id,lane};
      for(int pass=0;pass<3;++pass){
        auto gw=local_w,gb=local_b;
        dense_on(target,x.data(),rows,in,out,local_w.data(),local_b.data(),local_y.data());
        gradient_on(target,x.data(),local_d.data(),rows,in,out,gw.data(),gb.data());
        if(!eq(local_y,local_ref)||!eq(gw,local_rg)||!eq(gb,local_rb)){
          std::fprintf(stderr,"Lehrer HIP: CPU/GPU check failed on device %d stream %d\n",id,lane);
          ok.store(false,std::memory_order_relaxed); return;
        }
      }
    });
  }
  for(auto& t:pool) t.join();
  if(ok.load()) std::printf("HIP: %d GPU(s), %d independent streams per GPU checked\n",ngpu,lanes);
  return ok.load();
}
}
