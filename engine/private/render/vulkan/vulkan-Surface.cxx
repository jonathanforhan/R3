#if R3_VULKAN

#include "render/Surface.hpp"

// clang-format off
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/Instance.hpp"
#include "render/Window.hpp"

namespace R3 {

Surface::Surface(const SurfaceSpecification& spec)
    : m_spec(spec) {
    VkSurfaceKHR tmp;
    VkResult result =
        glfwCreateWindowSurface(m_spec.instance->as<VkInstance>(), m_spec.window->as<GLFWwindow*>(), nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);
    setHandle(tmp);
}

Surface::~Surface() {
    if (validHandle()) {
        vkDestroySurfaceKHR(m_spec.instance->as<VkInstance>(), as<VkSurfaceKHR>(), nullptr);
    }
}

} // namespace R3

#endif // R3_VULKAN