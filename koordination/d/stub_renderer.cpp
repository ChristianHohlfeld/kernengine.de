// (nur für den lokalen Harness: mesh_data.cpp verweist auf den Renderer, den kein Lauf hier ruft)
#include "kern/gfx/renderer.h"
bool kern::Renderer::upsert(kern::u32, const kern::MeshInput&) { __builtin_trap(); }
