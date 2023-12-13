#pragma once

#include <span>
#include "render/Buffer.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

template <std::integral T>
struct IndexBufferSpecification {
    Ref<const PhysicalDevice> physicalDevice;
    Ref<const LogicalDevice> logicalDevice;
    Ref<const CommandPool> commandPool;
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

    uint32 indexCount() const { return m_indexCount; }

private:
    IndexBufferSpecification<T> m_spec;
    uint32 m_indexCount = 0;
};

extern template class IndexBuffer<uint32>;
extern template class IndexBuffer<uint16>;

} // namespace R3
