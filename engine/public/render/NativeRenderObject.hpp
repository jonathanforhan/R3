#pragma once

namespace R3 {

class NativeRenderObject {
protected:
    NativeRenderObject() = default;

public:
    using Handle = void*;

    template <typename T = Handle>
    constexpr T handle() {
        return reinterpret_cast<T>(m_handle);
    }

    template <typename T = Handle>
    constexpr T handle() const {
        return reinterpret_cast<T>(m_handle);
    }

protected:
    template <typename T = Handle*>
    constexpr T handlePtr() {
        return reinterpret_cast<T>(&m_handle);
    }

    void setHandle(Handle handle) { m_handle = handle; }

private:
    Handle m_handle;
};

} // namespace R3