#pragma once

#include <R3>

namespace R3 {

struct R3_API Skin {
    std::vector<mat4> inverseBindMatrices;
    std::vector<usize> joints; // nodes used as joints
    usize skeleton = undefined;
};

} // namespace R3