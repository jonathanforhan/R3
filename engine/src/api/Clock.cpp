#include "api/Clock.hpp"

#if R3_OPENGL || R3_VULKAN
#include <GLFW/glfw3.h>
#else
#error "unimplented renderer"
#endif

namespace R3 {

double Clock::current_time() {
  return glfwGetTime();
}

} // namespace R3
