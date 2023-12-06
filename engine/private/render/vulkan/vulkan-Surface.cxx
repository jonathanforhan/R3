#if R3_VULKAN

#include "render/Surface.hpp"

// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
// clang-format on
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/Instance.hpp"
#include "render/Window.hpp"

namespace R3 {

void Surface::create(const SurfaceSpecification& spec) {
    CHECK(spec.instance != nullptr);
    CHECK(spec.window != nullptr);
    m_spec = spec;

    ENSURE(glfwCreateWindowSurface(m_spec.instance->handle<VkInstance>(),
                                   m_spec.window->handle<GLFWwindow*>(),
                                   nullptr,
                                   handlePtr<VkSurfaceKHR*>()) == VK_SUCCESS);
}

void Surface::destroy() {
    m_spec.instance->as<vk::Instance>().destroySurfaceKHR(as<vk::SurfaceKHR>());
}

} // namespace R3

#endif // R3_VULKAN