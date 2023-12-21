#pragma once

#include "render/Buffer.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct UniformBufferSpecification {
    Ref<const PhysicalDevice> physicalDevice;
    Ref<const LogicalDevice> logicalDevice;
    usize bufferSize;
};

class UniformBuffer : public Buffer {
public:
    UniformBuffer() = default;
    UniformBuffer(const UniformBufferSpecification& spec);
    UniformBuffer(UniformBuffer&&) noexcept = default;
    UniformBuffer& operator=(UniformBuffer&&) noexcept = default;
    ~UniformBuffer();

    void update(const void* buffer, usize size, usize offset);
    void flush();

private:
    Ref<const LogicalDevice> m_logicalDevice;
    usize m_bufferSize;
    void* m_mappedMemory = nullptr;
};

} // namespace R3
