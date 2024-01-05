#pragma once

#include <R3>

namespace R3 {

struct R3_API Skin {
    std::vector<mat4> inverseBindMatrices;
    std::vector<uint32> joints; // nodes used as joints
    uint32 skeleton = undefined;
};

} // namespace R3