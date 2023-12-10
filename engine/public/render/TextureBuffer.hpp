#pragma once

#include <string_view>
#include "render/Buffer.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct TextureBufferSpecification {
    Ref<const PhysicalDevice> physicalDevice;
    Ref<const LogicalDevice> logicalDevice;
    Ref<const CommandPool> commandPool;
    std::string_view path;
};

class TextureBuffer : public Buffer {
public:
    TextureBuffer() = default;
    TextureBuffer(const TextureBufferSpecification& spec);
    TextureBuffer(TextureBuffer&&) noexcept = default;
    TextureBuffer& operator=(TextureBuffer&&) noexcept = default;
    ~TextureBuffer();

private:
    TextureBufferSpecification m_spec;
};

} // namespace R3