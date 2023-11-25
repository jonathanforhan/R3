#pragma once

namespace R3 {

class NativeRenderObject {
protected:
    NativeRenderObject() = default;
public:
    using Handle = void*;
    
    constexpr Handle handle() { return m_handle; }

    template <typename T>
    constexpr T handle() {
        return (T)m_handle;
    }

    template <typename T>
    constexpr T* handlePtr() {
        return (T*)&m_handle;
    }

protected:
    void setHandle(Handle handle) { m_handle = handle; }

private:
    Handle m_handle;
};

} // namespace R3