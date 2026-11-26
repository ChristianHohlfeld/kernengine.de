#pragma once
#include <string>
#include <vector>
#include "kern/math/math.h"
namespace cases {
/// What the exports of pieces.cpp hand on: a text and bytes of a number.
std::string label_of(kern::i32 k);
std::vector<kern::u8> bytes_of(kern::i32 k);
}
