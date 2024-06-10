#include "vulkan-Surface.hxx"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "api/Assert.hpp"
#include "vulkan-Instance.hxx"

namespace R3::vulkan {

Surface::Surface(const SurfaceSpecification& spec)
    : m_instance(spec.instance) {
    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(m_instance, spec.window, nullptr, &surface);
    ENSURE(result == VK_SUCCESS);
    m_handle = surface;
}

Surface::~Surface() {
    vkDestroySurfaceKHR(m_instance, m_handle, nullptr);
}

} // namespace R3::vulkan