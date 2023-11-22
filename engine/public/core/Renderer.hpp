#pragma once
#include "api/Types.hpp"
#include "core/Framebuffer.hpp"
#include "core/Renderbuffer.hpp"

namespace R3 {

/// @brief Primitives used in draw call
enum class RenderPrimitive {
    Triangles,
    Lines,
    Points,
};

/// @brief depth function for depth buffer
enum class DepthFunction {
    Always,
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
};

/// @brief stencil function for stecil buffer
enum class StencilFunction {
    Always,
    Equal,
    NotEqual,
};

/// @brief stencil operation for stencil buffer
enum class StencilOperation {
    Keep,
    Zero,
    Replace,
    Increment,
    IncrementWrap,
    Decrement,
    DecrementWrap,
    Invert,
};

/// @brief Renderer class that provides the API for rendering geometry
/// The methods are implemented in private/hal/xx/Renderer.cxx so the API is universal
/// but the underlying Renderer can change by setting R3_[OPENGL|VULKAN|DX12]
/// @warning OpenGL is currently the only supported Renderer
class Renderer {
protected:
    /// @brief Constructable by Engine
    Renderer();
    friend class Engine; // friend so engine can construct

public:
    /// @brief Cull back faces
    void enableCulling();

    /// @brief Enable the Depth buffer and depth test
    void enableDepthTest();

    /// @brief Enable the Stencil buffer and stencil test
    void enableStencilTest();

    /// @brief Enable MSAA multisampling-antialiasing
    void enableMultisample();

    /// @brief Disable cull back faces
    void disableCulling();

    /// @brief Disable the Depth buffer and depth test
    void disableDepthTest();

    /// @brief Disable the Stencil buffer and stencil test
    void disableStencilTest();

    /// @brief Disable MSAA multisampling-antialiasing
    void disableMultisample();

    /// @brief Specify depthbuffer function
    /// @param func function
    void depthFunction(DepthFunction func);

    /// @brief Apply depth buffer bit mask
    /// @param mask
    void depthMask(bool b);

    /// @brief Specify the stencilbuffer function
    /// @param func function from R3::StencilFunction
    /// @param ref refernce
    /// @param mask stencil mask
    void stencilFunction(StencilFunction func, int32 ref, uint8 mask);

    /// @brief Apply stencil bit mask
    /// @param mask 0-255 mask
    void stencilMask(uint8 mask);

    /// @brief Specify the stencil operation
    /// @param stencilFail op on stencil failure
    /// @param depthFail op on depth failure
    /// @param pass op on passing
    void stencilOperation(StencilOperation stencilFail, StencilOperation depthFail, StencilOperation pass);

    /// @brief Execute necessary predraw functions like clearing color and depth buffers if enabled
    void predraw() const;

    /// @brief Draw elements using indices
    /// @param primitive primitive to use for rendering
    /// @param indiceCount number of indices
    void drawElements(RenderPrimitive primitive, uint32 indiceCount) const;

    /// @brief Draw array using only vertex data
    /// @param primitive primitive to use for rendering
    /// @param vertexCount number of vertices
    void drawArrays(RenderPrimitive primitive, uint32 vertexCount) const;

private:
    Framebuffer m_framebuffer;
    Renderbuffer m_renderbuffer;
};

} // namespace R3