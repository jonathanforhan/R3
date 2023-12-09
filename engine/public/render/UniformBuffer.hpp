#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/Buffer.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct UniformBufferSpecification {
    const PhysicalDevice* physicalDevice;
    const LogicalDevice* logicalDevice;
    usize bufferSize;
};

class UniformBuffer : public Buffer {
public:
    void create(const UniformBufferSpecification& spec);
    void destroy();

    void update(void* buffer, usize size);

private:
    UniformBufferSpecification m_spec;
    void* m_mappedMemory = nullptr;
};

} // namespace R3
