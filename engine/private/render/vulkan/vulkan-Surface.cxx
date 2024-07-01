#include "vulkan-Surface.hxx"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <api/Log.hpp>
#include <expected>
#include "vulkan-Instance.hxx"

namespace R3::vulkan {

Result<Surface> Surface::create(const SurfaceSpecification& spec) {
    Surface self;
    self.m_instance = spec.instance.vk();

    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(self.m_instance, spec.window, nullptr, &surface);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "glfwCreateWindowSurface failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    self.m_handle = surface;
    return self;
}

Surface::~Surface() {
    vkDestroySurfaceKHR(m_instance, m_handle, nullptr);
}

} // namespace R3::vulkan