#if R3_VULKAN

#include "render/Surface.hxx"

// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
// clang-format on
#include "api/Ensure.hpp"
#include "render/Instance.hxx"
#include "render/Window.hxx"

namespace R3 {

Surface::Surface(const SurfaceSpecification& spec)
    : m_instance(&spec.instance) {
    VkSurfaceKHR tmp;
    auto result = glfwCreateWindowSurface(m_instance->as<VkInstance>(), spec.window.as<GLFWwindow*>(), nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);
    setHandle(tmp);
}

Surface::~Surface() {
    if (validHandle()) {
        m_instance->as<vk::Instance>().destroySurfaceKHR(as<VkSurfaceKHR>());
    }
}

} // namespace R3

#endif // R3_VULKAN