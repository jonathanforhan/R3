#pragma once

#include <R3>
#include "render/CommandBuffer.hxx"

namespace R3 {

class UserInterface {
public:
    UserInterface() = delete;

    static void beginFrame();

    static void endFrame();

    static void populate();

    static void drawFrame(const CommandBuffer& commandBuffer);
};

} // namespace R3