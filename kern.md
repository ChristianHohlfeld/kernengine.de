# kern

url https://kernengine.de/kern/games/spawncity/web/index.html?menu=1 · kern true
started after 25.9 s

## foot

frames 19 in 10 s · frame ms p50 466.7 · p95 833.3 · p99 833.3 · max 3949.8 · draws/frame 149 · GL calls/frame 867
GL time per call name (ms in 10 s, calls): createVertexArray 17 (52) · createBuffer 15 (456) · bindBuffer 13 (2208) · bufferData 5 (564) · drawElementsInstanced 4 (2512) · bindVertexArray 4 (2880) · bufferSubData 4 (1273) · vertexAttribDivisor 3 (416) · uniform1i 2 (386) · uniform2f 1 (803) · bindFramebuffer 1 (190) · enableVertexAttribArray 1 (664)
long frames (> 150 ms) and their slowest GL calls: 667 ms [bindVertexArray 0, uniform1i 0, drawElementsInstanced 0, drawArrays 0, bindBuffer 0] | 600 ms [bufferSubData 0, uniform1i 0, uniformMatrix4fv 0, drawElementsInstanced 0, drawArrays 0] | 650 ms [bufferData 0, bufferSubData 0, createVertexArray 0, depthMask 0, enable 0] | 833 ms [bindBuffer 0, drawElementsInstanced 0, bindVertexArray 0, enableVertexAttribArray 0, createVertexArray 0] | 683 ms [bindVertexArray 0, bindBuffer 0, bufferData 0, vertexAttribPointer 0, useProgram 0] | 817 ms [drawElementsInstanced 1, bufferSubData 1, uniform2f 1, useProgram 0, bindTexture 0] | 650 ms [enableVertexAttribArray 0, bufferSubData 0, createVertexArray 0, bindBuffer 0, bufferData 0] | 700 ms [uniformMatrix4fv 1, bindVertexArray 0, drawElementsInstanced 0, bindTexture 0, bindBuffer 0] | 433 ms [bufferSubData 1, bindVertexArray 0, drawBuffers 0, uniform3f 0, getQueryParameter 0] | 467 ms [bindVertexArray 0, bufferSubData 0, getQueryParameter 0, uniform2f 0, bindBuffer 0] | 350 ms [bindBuffer 2, bindVertexArray 0, bufferSubData 0, disable 0, drawElementsInstanced 0] | 167 ms [uniform1i 1, drawArraysInstanced 1, vertexAttribDivisor 1, bufferSubData 0, bindVertexArray 0]
hud mode foot fps 22 tiles {"request":1102,"data":3014,"decoded":5296,"terrain":8170,"built":10679,"collision":25130,"work":383,"worst":8.3,"worstStep":"terrain","worstStepMs":8.3,"rebuilds":0,"relays":1,"uploads":102,"uploadKVerts":146} perf {"fps":12.2,"tier":4,"tierName":"maximal","auto":true,"show":false,"changes":0,"lights":0,"ms":{"logic":2.52,"physics":6.84,"stream":7.24,"draw":2.3,"update":18.9,"render":18.74,"gpu":1077.13}}

| self % | ms/s | function |
|---:|---:|---|
| 70.0 | 714.9 | (program) ·  |
| 23.5 | 240.1 | (idle) ·  |
| 0.9 | 8.7 | sc::Game::minimap_json() · spawncity.wasm-016e8ae2 |
| 0.7 | 7.5 | (garbage collector) ·  |
| 0.3 | 2.9 | b3BuildRecursive · spawncity.wasm-016e8ae2 |
| 0.2 | 1.8 | hypotf · spawncity.wasm-016e8ae2 |
| 0.2 | 1.7 | sbrk · spawncity.wasm-016e8ae2 |
| 0.2 | 1.6 | sc::TrafficRules::rebuild(std::__2::vector<sc::Lane, std::__2::allocator<sc::Lane>> const&) · spawncity.wasm-016e8ae2 |
| 0.1 | 1.4 | b3RayCastMesh · spawncity.wasm-016e8ae2 |
| 0.1 | 1.3 | sc::TrafficRules::rebuild(std::__2::vector<sc::Lane, std::__2::allocator<sc::Lane>> const&)::$_0::operator()(kern::Vec2) const · spawncity.wasm-016e8ae2 |
| 0.1 | 1.2 | bindBuffer ·  |
| 0.1 | 1.2 | querySelectorAll ·  |
| 0.1 | 1.1 | b3CreateMesh · spawncity.wasm-016e8ae2 |
| 0.1 | 1.1 | printf_core · spawncity.wasm-016e8ae2 |
| 0.1 | 1.1 | sc::Game::map_json() · spawncity.wasm-016e8ae2 |
| 0.1 | 1.1 | kern::Bvh::build(float const*, unsigned int, unsigned int const*, unsigned int, unsigned int) · spawncity.wasm-016e8ae2 |
| 0.1 | 1.1 | sc::Game::step_tow(float) · spawncity.wasm-016e8ae2 |
| 0.1 | 1.0 | update · view.mjs |
| 0.1 | 0.9 | b3DynamicTree_RayCast · spawncity.wasm-016e8ae2 |
| 0.1 | 0.9 | proto.<computed> ·  |
| 0.1 | 0.7 | dlmalloc · spawncity.wasm-016e8ae2 |
| 0.1 | 0.6 | now ·  |
| 0.1 | 0.6 | kern::SunShadowMap::draw(kern::Span<kern::Mesh* const>, kern::Vec3, kern::Vec3, unsigned int, float) · spawncity.wasm-016e8ae2 |
| 0.1 | 0.6 | enable · kern.mjs |
| 0.1 | 0.6 | RayCastCallback · spawncity.wasm-016e8ae2 |
| 0.1 | 0.6 | b3QueryMesh · spawncity.wasm-016e8ae2 |
| 0.1 | 0.6 | update · host.mjs |
| 0.1 | 0.6 | sc::Car::step(kern::physics::World&, kern::physics::Body, float, sc::CarInput const&) · spawncity.wasm-016e8ae2 |
| 0.1 | 0.5 | minimap · view.mjs |
| 0.1 | 0.5 | sc::Car::reapply(kern::physics::World&, kern::physics::Body) const · spawncity.wasm-016e8ae2 |
| 0.1 | 0.5 | enableVertexAttribArray · kern.mjs |
| 0.0 | 0.5 | void std::__2::vector<unsigned char, std::__2::allocator<unsigned char>>::__assign_with_size[abi:nn190105]<unsigned char const*, unsigned char const*>(unsigned char const*, unsigned char const*, long) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | bufferData ·  |
| 0.0 | 0.4 | std::__2::__wrap_iter<float*> std::__2::vector<float, std::__2::allocator<float>>::__insert_with_size[abi:nn190105]<float const*, float const*>(std::__2::__wrap_iter<float const*>, float const*, float const*, long) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | tick · host.mjs |
| 0.0 | 0.4 | b3DynamicTree_Refit · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | b3PushContacts_MeshWideW4 · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | b3CreateHull · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | b3ExecuteBlock · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | P · view.mjs |
| 0.0 | 0.3 | dlfree · spawncity.wasm-016e8ae2 |
| 0.0 | 0.3 | vertexAttribDivisor ·  |
| 0.0 | 0.3 | update · gaps_hud.mjs |
| 0.0 | 0.3 | void std::__2::vector<float, std::__2::allocator<float>>::__assign_with_size[abi:nn190105]<float const*, float const*>(float const*, float const*, long) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.3 | b3DynamicTree_Query · spawncity.wasm-016e8ae2 |

## drone

frames 17 in 10 s · frame ms p50 483.3 · p95 3449.8 · p99 3449.8 · max 3699.8 · draws/frame 231 · GL calls/frame 1394
GL time per call name (ms in 10 s, calls): bindVertexArray 7 (4209) · bufferSubData 7 (1450) · drawElementsInstanced 6 (3754) · bufferData 6 (1002) · uniform1i 4 (344) · enableVertexAttribArray 3 (1458) · vertexAttribPointer 3 (1458) · bindBuffer 3 (3041) · createVertexArray 2 (117) · uniform2f 2 (1117) · createBuffer 1 (990) · cullFace 1 (374)
long frames (> 150 ms) and their slowest GL calls: 533 ms [bindVertexArray 2, drawElementsInstanced 0, createBuffer 0, bindFramebuffer 0, depthMask 0] | 483 ms [uniform1i 3, bindVertexArray 0, drawElementsInstanced 0, bufferData 0, enableVertexAttribArray 0] | 3700 ms [bufferData 1, bindVertexArray 0, bindBuffer 0, createVertexArray 0, enableVertexAttribArray 0] | 450 ms [bufferSubData 2, bufferData 0, createBuffer 0, vertexAttribPointer 0, bindVertexArray 0] | 167 ms [deleteBuffer 1, bindBuffer 0, drawElementsInstanced 0, bufferData 0, bindVertexArray 0] | 183 ms [bufferSubData 1, bufferData 0, uniform1i 0, bindVertexArray 0, createVertexArray 0] | 333 ms [drawElementsInstanced 0, bufferSubData 0, bindVertexArray 0, getQueryParameter 0, bindBuffer 0] | 583 ms [bufferSubData 1, uniform2f 0, useProgram 0, drawElementsInstanced 0, createVertexArray 0] | 583 ms [bindVertexArray 0, bindBuffer 0, drawElementsInstanced 0, uniform2f 0, vertexAttribPointer 0] | 483 ms [vertexAttribPointer 1, bindVertexArray 0, bindBuffer 0, bufferData 0, vertexAttribDivisor 0] | 500 ms [bindVertexArray 0, vertexAttribPointer 0, bufferSubData 0, bufferData 0, vertexAttribDivisor 0] | 533 ms [bindVertexArray 1, bufferData 0, uniform2f 0, enable 0, drawElementsInstanced 0]
hud mode drone fps 14 tiles {"request":1102,"data":3014,"decoded":5296,"terrain":8170,"built":10679,"collision":25130,"work":532,"worst":4.6,"worstStep":"roads","worstStepMs":2.7,"rebuilds":2,"relays":2,"uploads":215,"uploadKVerts":271} perf {"fps":10.4,"tier":3,"tierName":"hoch","auto":true,"show":false,"changes":1,"lights":0,"ms":{"logic":0.76,"physics":24.9,"stream":4.46,"draw":1.86,"update":31.98,"render":2.92,"gpu":506.6}}

| self % | ms/s | function |
|---:|---:|---|
| 48.5 | 603.1 | (program) ·  |
| 44.9 | 558.4 | (idle) ·  |
| 0.7 | 8.5 | sc::Game::minimap_json() · spawncity.wasm-016e8ae2 |
| 0.6 | 7.3 | sc::Game::traffic_advance(sc::TrafficCar&) · spawncity.wasm-016e8ae2 |
| 0.3 | 4.0 | b3SolveContacts_MeshWideW4 · spawncity.wasm-016e8ae2 |
| 0.3 | 3.7 | hypotf · spawncity.wasm-016e8ae2 |
| 0.2 | 2.8 | (garbage collector) ·  |
| 0.2 | 2.8 | proto.<computed> ·  |
| 0.2 | 2.6 | update · gaps_hud.mjs |
| 0.2 | 2.6 | b3PushContacts_MeshWideW4 · spawncity.wasm-016e8ae2 |
| 0.2 | 2.3 | sc::Car::step(kern::physics::World&, kern::physics::Body, float, sc::CarInput const&) · spawncity.wasm-016e8ae2 |
| 0.2 | 2.1 | printf_core · spawncity.wasm-016e8ae2 |
| 0.2 | 2.0 | b3ApplyRestitution_MeshWideW4 · spawncity.wasm-016e8ae2 |
| 0.1 | 1.7 | b3PrepareContacts_MeshWideW4 · spawncity.wasm-016e8ae2 |
| 0.1 | 1.7 | b3RayCastMesh · spawncity.wasm-016e8ae2 |
| 0.1 | 1.6 | b3ExecuteBlock · spawncity.wasm-016e8ae2 |
| 0.1 | 1.5 | b3DynamicTree_RayCast · spawncity.wasm-016e8ae2 |
| 0.1 | 1.2 | kern::Bvh::build(float const*, unsigned int, unsigned int const*, unsigned int, unsigned int) · spawncity.wasm-016e8ae2 |
| 0.1 | 1.1 | b3CollideTask · spawncity.wasm-016e8ae2 |
| 0.1 | 1.1 | b3WarmStartContacts_MeshWideW4 · spawncity.wasm-016e8ae2 |
| 0.1 | 0.9 | b3ComputeMeshManifolds · spawncity.wasm-016e8ae2 |
| 0.1 | 0.9 | bufferSubData ·  |
| 0.1 | 0.8 | sc::Terrain::update(kern::Vec3, kern::Vec3, double) · spawncity.wasm-016e8ae2 |
| 0.1 | 0.8 | querySelectorAll ·  |
| 0.1 | 0.7 | sc::Game::map_json() · spawncity.wasm-016e8ae2 |
| 0.1 | 0.7 | sc::Terrain::node_height(float, float) const · spawncity.wasm-016e8ae2 |
| 0.1 | 0.7 | update · host.mjs |
| 0.1 | 0.6 | sc::Game::traffic_rules(int, kern::Vec3, kern::Vec3, float) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.6 | minimap · view.mjs |
| 0.0 | 0.6 | std::__2::__wrap_iter<float*> std::__2::vector<float, std::__2::allocator<float>>::__insert_with_size[abi:nn190105]<float const*, float const*>(std::__2::__wrap_iter<float const*>, float const*, float const*, long) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.6 | bindVertexArray ·  |
| 0.0 | 0.6 | b3CollideProxyAndSubtree · spawncity.wasm-016e8ae2 |
| 0.0 | 0.6 | b3QueryMesh · spawncity.wasm-016e8ae2 |
| 0.0 | 0.5 | b3FinalizeBodiesTask · spawncity.wasm-016e8ae2 |
| 0.0 | 0.5 | update · view.mjs |
| 0.0 | 0.5 | vertexAttribPointer ·  |
| 0.0 | 0.5 | __fwritex · spawncity.wasm-016e8ae2 |
| 0.0 | 0.5 | bufferData ·  |
| 0.0 | 0.4 | enableVertexAttribArray · kern.mjs |
| 0.0 | 0.4 | sc::Game::traffic_far_step(sc::TrafficCar&, float) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | b3GatherCrossSeeds · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | kern::physics::World::apply_force(kern::physics::Body, kern::Vec3, kern::Vec3) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | dlmalloc · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | sc::Game::step_tow(float) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | b3Solve · spawncity.wasm-016e8ae2 |

## car

frames 13 in 10 s · frame ms p50 683.4 · p95 4749.8 · p99 4749.8 · max 6266.4 · draws/frame 281 · GL calls/frame 1857
GL time per call name (ms in 10 s, calls): bufferData 9 (1358) · drawElementsInstanced 7 (3589) · bindVertexArray 5 (4001) · vertexAttribPointer 4 (1971) · bindBuffer 3 (2535) · bufferSubData 2 (769) · uniform2f 2 (1347) · enableVertexAttribArray 2 (1971) · vertexAttribDivisor 1 (1232) · createVertexArray 1 (154) · deleteBuffer 1 (890) · createBuffer 1 (1355)
long frames (> 150 ms) and their slowest GL calls: 4750 ms [bufferData 1, createBuffer 0, enableVertexAttribArray 0, disable 0, drawElementsInstanced 0] | 1067 ms [bindVertexArray 0, enableVertexAttribArray 0, bufferData 0, vertexAttribPointer 0, drawElementsInstanced 0] | 783 ms [drawElementsInstanced 4, bufferData 3, createVertexArray 0, bindVertexArray 0, enableVertexAttribArray 0] | 783 ms [vertexAttribDivisor 1, bindBuffer 1, bindVertexArray 1, bufferData 0, drawElementsInstanced 0] | 283 ms [deleteBuffer 1, bindBuffer 1, bufferData 0, vertexAttribPointer 0, bufferSubData 0] | 167 ms [bufferData 2, enableVertexAttribArray 0, deleteBuffer 0, bufferSubData 0, deleteVertexArray 0] | 683 ms [disable 1, bufferSubData 0, bindVertexArray 0, createBuffer 0, bufferData 0] | 6266 ms [uniform2f 1, bindBuffer 1, bufferSubData 0, bindVertexArray 0, useProgram 0] | 2333 ms [drawElementsInstanced 1, bindVertexArray 1, getParameter 0, clear 0, uniform2f 0]
hud mode car fps 17 tiles {"request":1102,"data":3014,"decoded":5296,"terrain":8170,"built":10679,"collision":25130,"work":653,"worst":4.2,"worstStep":"","worstStepMs":0,"rebuilds":4,"relays":3,"uploads":371,"uploadKVerts":498} perf {"fps":10.5,"tier":3,"tierName":"hoch","auto":true,"show":false,"changes":1,"lights":0,"ms":{"logic":0.91,"physics":15.88,"stream":4.59,"draw":1.66,"update":23.03,"render":2.62,"gpu":735.43}}

| self % | ms/s | function |
|---:|---:|---|
| 60.8 | 698.5 | (program) ·  |
| 34.2 | 393.2 | (idle) ·  |
| 0.8 | 9.1 | sc::Game::minimap_json() · spawncity.wasm-016e8ae2 |
| 0.6 | 6.4 | sbrk · spawncity.wasm-016e8ae2 |
| 0.4 | 4.1 | (garbage collector) ·  |
| 0.3 | 4.0 | sc::Game::traffic_advance(sc::TrafficCar&) · spawncity.wasm-016e8ae2 |
| 0.2 | 2.6 | proto.<computed> ·  |
| 0.2 | 2.3 | hypotf · spawncity.wasm-016e8ae2 |
| 0.1 | 1.5 | printf_core · spawncity.wasm-016e8ae2 |
| 0.1 | 1.4 | minimap · view.mjs |
| 0.1 | 1.2 | b3DynamicTree_RayCast · spawncity.wasm-016e8ae2 |
| 0.1 | 1.0 | sc::Car::step(kern::physics::World&, kern::physics::Body, float, sc::CarInput const&) · spawncity.wasm-016e8ae2 |
| 0.1 | 0.9 | sc::Terrain::update(kern::Vec3, kern::Vec3, double) · spawncity.wasm-016e8ae2 |
| 0.1 | 0.7 | std::__2::__wrap_iter<float*> std::__2::vector<float, std::__2::allocator<float>>::__insert_with_size[abi:nn190105]<float const*, float const*>(std::__2::__wrap_iter<float const*>, float const*, float const*, long) · spawncity.wasm-016e8ae2 |
| 0.1 | 0.7 | sc::Game::step_tow(float) · spawncity.wasm-016e8ae2 |
| 0.1 | 0.6 | querySelectorAll ·  |
| 0.1 | 0.6 | update · gaps_hud.mjs |
| 0.1 | 0.6 | b3ExecuteBlock · spawncity.wasm-016e8ae2 |
| 0.1 | 0.6 | b3FinalizeBodiesTask · spawncity.wasm-016e8ae2 |
| 0.0 | 0.5 | b3ComputeMeshManifolds · spawncity.wasm-016e8ae2 |
| 0.0 | 0.5 | sc::Game::map_json() · spawncity.wasm-016e8ae2 |
| 0.0 | 0.5 | kern::Mat4::operator*(kern::Mat4 const&) const · spawncity.wasm-016e8ae2 |
| 0.0 | 0.5 | b3RayCastMesh · spawncity.wasm-016e8ae2 |
| 0.0 | 0.5 | fmtPts · view_modes.mjs |
| 0.0 | 0.5 | $ · view_modes.mjs |
| 0.0 | 0.4 | bufferData ·  |
| 0.0 | 0.4 | void std::__2::vector<unsigned char, std::__2::allocator<unsigned char>>::__assign_with_size[abi:nn190105]<unsigned char const*, unsigned char const*>(unsigned char const*, unsigned char const*, long) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | sc::Car::push(kern::physics::World&, kern::physics::Body, kern::Quat const&, kern::Vec3, kern::Vec3, kern::Vec3, bool) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.4 | done · jobs.mjs |
| 0.0 | 0.4 | now ·  |
| 0.0 | 0.3 | b3ComputeHullAABB · spawncity.wasm-016e8ae2 |
| 0.0 | 0.3 | update · host.mjs |
| 0.0 | 0.3 | deleteBuffer ·  |
| 0.0 | 0.3 | speedLines · view_modes.mjs |
| 0.0 | 0.3 | int std::__2::__function::__policy_invoker<int (kern::logic::Context&)>::__call_impl[abi:nn190105]<std::__2::__function::__default_alloc_func<sc::(anonymous namespace)::register_node()::$_1, int (kern::logic::Context&)>>(std::__2::__function::__policy_storage const*, kern::logic::Context&) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.2 | b3CollideTask · spawncity.wasm-016e8ae2 |
| 0.0 | 0.2 | drawElementsInstanced ·  |
| 0.0 | 0.2 | kern::instanced_bounds(kern::Sphere const&, float const*, unsigned int) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.2 | kern::bounding_sphere(float const*, unsigned int) · spawncity.wasm-016e8ae2 |
| 0.0 | 0.2 | b3PushContacts_MeshWideW4 · spawncity.wasm-016e8ae2 |
| 0.0 | 0.2 | (anonymous) · original_sfx.mjs |
| 0.0 | 0.2 | sc::Terrain::node_height(float, float) const · spawncity.wasm-016e8ae2 |
| 0.0 | 0.2 | (anonymous) · view.mjs |
| 0.0 | 0.2 | b3SolveContacts_ConvexW4 · spawncity.wasm-016e8ae2 |
| 0.0 | 0.2 | bindVertexArray ·  |
