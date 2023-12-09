#pragma once

#include <span>
#include "render/Buffer.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

template <std::integral T>
struct IndexBufferSpecification {
    const PhysicalDevice* physicalDevice;
    const LogicalDevice* logicalDevice;
    const CommandPool* commandPool;
    std::span<T> indices;
};

template <std::integral T>
class IndexBuffer : public Buffer {
public:
    void create(const IndexBufferSpecification<T>& spec);
    void destroy();

    uint32 indexCount() const { return m_indexCount; }

private:
    IndexBufferSpecification<T> m_spec;
    uint32 m_indexCount = 0;
};

extern template class IndexBuffer<uint32>;
extern template class IndexBuffer<uint16>;

} // namespace R3