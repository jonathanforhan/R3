#pragma once

#include "render/NativeRenderObject.hpp"

namespace R3 {

class Buffer : public NativeRenderObject {
protected:
    Buffer() = default;

    template <typename T>
    T buffer() {
        return reinterpret_cast<T>(m_deviceMemory);
    }

    template <typename T>
    T buffer() const {
        return reinterpret_cast<T>(m_deviceMemory);
    }

    template <typename T>
    T bufferPtr() {
        return reinterpret_cast<T>(&m_deviceMemory);
    }

private:
    // points to device (GPU) memory allocated for buffer
    void* m_deviceMemory;
};

} // namespace R3