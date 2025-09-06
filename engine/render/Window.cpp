#include "Window.hpp"

#if WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#else
#define GLFW_EXPOSE_NATIVE_X11 1
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <array>
#include <string_view>
#include "api/Assert.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "core/EventHandler.hpp"
#include "core/Log.hpp"
#include "input/InputCodes.hpp"
#include "input/InputEvents.hpp"
#include "render/WindowEvents.hpp"

namespace R3 {

#define KEY_TO_INDEX(_Key) ((usize)(((int)_Key) - (int)Key::Space))
#define VALID_KEY(_Key)    (_Key >= (int)Key::Space && (_Key) <= (int)Key::Menu)

void Window::setTitle(std::string_view title) {
    glfwSetWindowTitle(m_window, title.data());
}

void Window::show() {
    glfwShowWindow(m_window);
}

void Window::hide() {
    glfwHideWindow(m_window);
}

usize2 Window::size() const {
    int32 w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return usize2(static_cast<usize>(w), static_cast<usize>(h));
}

usize Window::width() const {
    int32 width;
    glfwGetWindowSize(m_window, &width, nullptr);
    return static_cast<usize>(width);
}

usize Window::height() const {
    int32 height;
    glfwGetWindowSize(m_window, nullptr, &height);
    return static_cast<usize>(height);
}

usize2 Window::framebufferSize() const {
    int32 w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    return usize2(static_cast<usize>(w), static_cast<usize>(h));
}

usize Window::framebufferWidth() const {
    int32 width;
    glfwGetFramebufferSize(m_window, &width, nullptr);
    return static_cast<usize>(width);
}

usize Window::framebufferHeight() const {
    int32 height;
    glfwGetFramebufferSize(m_window, nullptr, &height);
    return static_cast<usize>(height);
}

void Window::setSize(usize2 extent) {
    glfwSetWindowSize(m_window, static_cast<int>(extent.x), static_cast<int>(extent.y));
}

void Window::setSize(usize width, usize height) {
    glfwSetWindowSize(m_window, static_cast<int>(width), static_cast<int>(height));
}

ivec2 Window::position() const {
    int x, y;
    glfwGetWindowPos(m_window, &x, &y);
    return ivec2(x, y);
}

void Window::setPosition(ivec2 position) {
    glfwSetWindowPos(m_window, position.x, position.y);
}

void Window::setPosition(int32 x, int32 y) {
    glfwSetWindowPos(m_window, x, y);
}

float Window::aspectRatio() const {
    fvec2 extent{static_cast<fvec2>(size())};
    return extent.x / extent.y;
}

fvec2 Window::contentScale() const {
    float x, y;
    glfwGetWindowContentScale(m_window, &x, &y);
    return fvec2(x, y);
}

bool Window::isMinimized() const {
    int32 width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return !(width && height);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

bool Window::shouldResize() const {
    return m_shouldResize;
}

void Window::setShouldResize(bool b) {
    m_shouldResize = b;
}

bool Window::focused() const {
    int focused = glfwGetWindowAttrib(m_window, GLFW_FOCUSED);
    return static_cast<bool>(focused);
}

bool Window::uiFocused() const {
    return m_uiFocused;
}

bool Window::keyPressed(Key key) const {
    int pressed = glfwGetKey(m_window, (int)key); // update key state
    return pressed == GLFW_PRESS;
}

bool Window::mouseButtonPressed(MouseButton button) const {
    int pressed = glfwGetMouseButton(m_window, (int)button);
    return pressed == GLFW_PRESS;
}

dvec2 Window::cursorPosition() const {
    dvec2 pos;
    glfwGetCursorPos(m_window, &pos.x, &pos.y);
    return pos;
}

void* Window::native() {
#if WIN32
    return glfwGetWin32Window(m_window);
#else
    return glfwGetX11Window(m_window);
#endif
}

GLFWwindow* Window::glfw() {
    return m_window;
}

const GLFWwindow* Window::glfw() const {
    return m_window;
}

void Window::kill() {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void Window::update(bool uiFocused) {
    m_uiFocused = uiFocused;
    glfwPollEvents();
}

void Window::setupCallbacks() {
    //--- Error Callback
    auto errorCallback = [](int code, const char* msg) { LOG_ERROR("glfw error code: {}, {}", code, msg); };
    glfwSetErrorCallback(errorCallback);

    //--- Keyboard Input Callback
    auto keyCallback = [](GLFWwindow* window, int key, int, int action, int mods) {
        R3_ASSERT(VALID_KEY(key));

        auto* _this = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window));
        if (_this->m_uiFocused) {
            return;
        }
        _this->m_keyStates[KEY_TO_INDEX(key)] = (action != GLFW_RELEASE);

        switch (action) {
            case GLFW_PRESS:
                GEventHandler()->emplace<KeyboardEvent>("key-press", Key(key), InputModifiers(mods));
                break;
            case GLFW_REPEAT:
                GEventHandler()->emplace<KeyboardEvent>("key-repeat", Key(key), InputModifiers(mods));
                break;
            case GLFW_RELEASE:
                GEventHandler()->emplace<KeyboardEvent>("key-release", Key(key), InputModifiers(mods));
                break;
            default:
                return;
        }
    };
    glfwSetKeyCallback(m_window, keyCallback);

    //--- Mouse Button Callback
    auto mouseCallback = [](GLFWwindow* window, int button, int action, int mods) {
        auto* _this = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window));
        if (_this->m_uiFocused) {
            // release all previously pressed keys
            for (Key key = Key::Space; bool state : _this->m_keyStates) {
                if (state) {
                    GEventHandler()->emplace<KeyboardEvent>("key-release", key, InputModifiers(mods));
                }
                key = Key((uint16)key + 1);
            }
            return;
        }

        switch (action) {
            case GLFW_PRESS:
                GEventHandler()->emplace<MouseButtonEvent>("mouse-press", MouseButton(button), InputModifiers(mods));
                break;
            case GLFW_RELEASE:
                GEventHandler()->emplace<MouseButtonEvent>("mouse-release", MouseButton(button), InputModifiers(mods));
                break;
            default:
                return;
        }
    };
    glfwSetMouseButtonCallback(m_window, mouseCallback);

    glfwFocusWindow(m_window);

    //--- Mouse Scroll Callback
    auto scrollCallback = [](GLFWwindow* window, double xoffset, double yoffset) {
        auto* _this = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window));
        if (_this->m_uiFocused) {
            return;
        }
        GEventHandler()->emplace<MouseScrollEvent>("mouse-scroll", xoffset, yoffset);
    };
    glfwSetMouseButtonCallback(m_window, mouseCallback);

    //--- Cursor Input Callback
    auto cursorCallback = [](GLFWwindow*, double x, double y) {
        GEventHandler()->emplace<MouseCursorEvent>("cursor-move", x, y);
    };
    glfwSetCursorPosCallback(m_window, cursorCallback);

    //--- Window Resize Callback
    auto resizeCallback = [](GLFWwindow* window, int width, int height) {
        auto* _this = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window));
        _this->setShouldResize(true);
        GEventHandler()->emplace<WindowResizeEvent>("window-resize", (int32)width, (int32)height);
    };
    glfwSetFramebufferSizeCallback(m_window, resizeCallback);

    //--- Window Focus Callback
    auto focusCallback = [](GLFWwindow*, int focused) {
        if (focused) {
            GEventHandler()->emplace<WindowFocusEvent>("window-focus", (bool)focused);
        }
    };
    glfwSetWindowFocusCallback(m_window, focusCallback);

    //--- Window Content Scale Callback
    auto contentScaleCallback = [](GLFWwindow*, float xscale, float yscale) {
        GEventHandler()->emplace<WindowContentScaleEvent>("window-content-scale", xscale, yscale);
    };
    glfwSetWindowContentScaleCallback(m_window, contentScaleCallback);

    //--- Window Close Callback
    auto windowCloseCallback = [](GLFWwindow*) { GEventHandler()->emplace<WindowCloseEvent>("window-close"); };
    glfwSetWindowCloseCallback(m_window, windowCloseCallback);
}

} // namespace R3
