#pragma once

#include <span>
#include "render/Buffer.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

template <std::integral T>
struct IndexBufferSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    const CommandPool& commandPool;
    std::span<const T> indices;
};

template <std::integral T>
class IndexBuffer : public Buffer {
public:
    IndexBuffer() = default;
    IndexBuffer(const IndexBufferSpecification<T>& spec);
    IndexBuffer(IndexBuffer&&) noexcept = default;
    IndexBuffer& operator=(IndexBuffer&&) noexcept = default;
    ~IndexBuffer();

    uint32 count() const { return m_indexCount; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    uint32 m_indexCount = 0;
};

extern template class IndexBuffer<uint32>;
extern template class IndexBuffer<uint16>;

} // namespace R3
