#pragma once

#include <R3>
#include "render/model/KeyFrame.hpp"

namespace R3 {

struct Animation {
    float currentTime = 0.0f;
    float maxTime = 0.0f;
    std::vector<KeyFrame> keyFrames;
};

} // namespace R3