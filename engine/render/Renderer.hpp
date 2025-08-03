#pragma once

#include "CommandAllocator.hpp"
#include "RenderContext.hpp"
#include "Swapchain.hpp"

namespace R3 {

class Renderer {
public:
    Renderer() = default;

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&&) noexcept            = delete;
    Renderer& operator=(Renderer&&) noexcept = delete;

private:
};

} // namespace R3