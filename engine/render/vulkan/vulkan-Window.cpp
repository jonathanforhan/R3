#include "render/Window.hpp"

#if WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#else
#define GLFW_EXPOSE_NATIVE_X11 1
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <string_view>
#include "api/Assert.hpp"
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "core/EventHandler.hpp"
#include "core/Log.hpp"
#include "input/InputCodes.hpp"
#include "input/InputEvents.hpp"
#include "render/WindowEvents.hpp"

#if R3_EDITOR
#include <imgui.h>
#endif
#include <array>

namespace R3 {

#if R3_EDITOR
#define WAS_UI_CAPTURED()       (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse)
#define WAS_UI_MOUSE_CAPTURED() (ImGui::GetIO().WantCaptureMouse)
#else
#define WAS_UI_CAPTURED()       false
#define WAS_UI_MOUSE_CAPTURED() false
#endif

#define KEY_TO_INDEX(_Key) ((usize)(_Key - (int)Key::Space))
#define VALID_KEY(_Key)    (_Key >= (int)Key::Space && _Key <= (int)Key::Menu)

Window::Window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
    // glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();

    int monitorX, monitorY;
    glfwGetMonitorPos(primary, &monitorX, &monitorY);

    float contentScaleX, contentScaleY;
    glfwGetMonitorContentScale(primary, &contentScaleX, &contentScaleY);

    const GLFWvidmode* vidmode = glfwGetVideoMode(primary);
    const int width            = static_cast<int>(vidmode->width / contentScaleX);
    const int height           = static_cast<int>(vidmode->height / contentScaleY);
    static const char* title   = "R3";

    if (!(m_window = glfwCreateWindow(width, height, title, nullptr, nullptr))) {
        glfwTerminate();
        throw Exception("GLFW window creation failure");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GLFW_TRUE);

    // center glfw window on create
    const int centerX = monitorX + (vidmode->width - width) / 2;
    const int centerY = monitorY + (vidmode->height - height) / 2;
    glfwSetWindowPos(m_window, centerX, centerY);

    //--- Error Callback
    auto errorCallback = [](int code, const char* msg) { LOG_ERROR("glfw error code: {}, {}", code, msg); };
    glfwSetErrorCallback(errorCallback);

    //--- Keyboard Input Callback
    auto keyCallback = [](GLFWwindow* window, int key, int, int action, int mods) {
        R3_ASSERT(VALID_KEY(key));

        if (WAS_UI_CAPTURED()) {
            return;
        }

        auto* _this{reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window))};
        _this->m_keyStates[KEY_TO_INDEX(key)] = (action != GLFW_RELEASE);

        switch (action) {
            case GLFW_PRESS:
                EventHandler()->emplace<KeyboardEvent>("key-press", Key(key), InputModifiers(mods));
                break;
            case GLFW_REPEAT:
                EventHandler()->emplace<KeyboardEvent>("key-repeat", Key(key), InputModifiers(mods));
                break;
            case GLFW_RELEASE:
                EventHandler()->emplace<KeyboardEvent>("key-release", Key(key), InputModifiers(mods));
                break;
            default:
                return;
        }
    };
    glfwSetKeyCallback(m_window, keyCallback);

    //--- Mouse Button Callback
    auto mouseCallback = [](GLFWwindow* window, int button, int action, int mods) {
        if (WAS_UI_MOUSE_CAPTURED()) {
            auto* _this = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window));
            for (Key key = Key::Space; bool state : _this->m_keyStates) {
                if (state) {
                    EventHandler()->emplace<KeyboardEvent>("key-release", key, InputModifiers(mods));
                }
                key = Key((uint16)key + 1);
            }
            return;
        }

        switch (action) {
            case GLFW_PRESS:
                EventHandler()->emplace<MouseButtonEvent>("mouse-press", MouseButton(button), InputModifiers(mods));
                break;
            case GLFW_RELEASE:
                EventHandler()->emplace<MouseButtonEvent>("mouse-release", MouseButton(button), InputModifiers(mods));
                break;
            default:
                return;
        }
    };
    glfwSetMouseButtonCallback(m_window, mouseCallback);

    glfwFocusWindow(m_window);

    //--- Mouse Scroll Callback
    auto scrollCallback = [](GLFWwindow*, double xoffset, double yoffset) {
        if (WAS_UI_MOUSE_CAPTURED()) {
            return;
        }

        EventHandler()->emplace<MouseScrollEvent>("mouse-scroll", xoffset, yoffset);
    };
    glfwSetMouseButtonCallback(m_window, mouseCallback);

    //--- Cursor Input Callback
    auto cursorCallback = [](GLFWwindow* window, double x, double y) {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        double xpos = static_cast<double>(x) / static_cast<double>(w);
        double ypos = static_cast<double>(y) / static_cast<double>(h);
        EventHandler()->emplace<MouseCursorEvent>("cursor-move", xpos, ypos);
    };
    glfwSetCursorPosCallback(m_window, cursorCallback);

    //--- Window Resize Callback
    auto resizeCallback = [](GLFWwindow* window, int width, int height) {
        auto* _this = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window));
        _this->setShouldResize(true);
        EventHandler()->emplace<WindowResizeEvent>("window-resize", (int32)width, (int32)height);
    };
    glfwSetFramebufferSizeCallback(m_window, resizeCallback);

    //--- Window Focus Callback
    auto focusCallback = [](GLFWwindow*, int focused) {
        if (focused) {
            EventHandler()->emplace<WindowFocusEvent>("window-focus", (bool)focused);
        }
    };
    glfwSetWindowFocusCallback(m_window, focusCallback);

    //--- Window Content Scale Callback
    auto contentScaleCallback = [](GLFWwindow*, float xscale, float yscale) {
        EventHandler()->emplace<WindowContentScaleEvent>("window-content-scale", xscale, yscale);
    };
    glfwSetWindowContentScaleCallback(m_window, contentScaleCallback);

    //--- Window Close Callback
    auto windowCloseCallback = [](GLFWwindow*) { EventHandler()->emplace<WindowCloseEvent>("window-close"); };
    glfwSetWindowCloseCallback(m_window, windowCloseCallback);

    /* Add callback to show window once the first frame is rendered, this prevents white screen */
    EventHandler()->bindEventListener("frame-done", [this]() noexcept {
        show();
        return true; // remove after first call
    });
}

Window::~Window() noexcept {
    if (m_window) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_window = nullptr;
    }
}

void Window::setTitle(std::string_view title) {
    glfwSetWindowTitle(m_window, title.data());
}

void Window::show() {
    glfwShowWindow(m_window);
}

void Window::hide() {
    glfwHideWindow(m_window);
}

ivec2 Window::size() const {
    int32 w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return ivec2(w, h);
}

int32 Window::width() const {
    int32 width;
    glfwGetWindowSize(m_window, &width, nullptr);
    return width;
}

int32 Window::height() const {
    int32 height;
    glfwGetWindowSize(m_window, nullptr, &height);
    return height;
}

ivec2 Window::framebufferSize() const {
    int32 w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    return ivec2(w, h);
}

int32 Window::framebufferWidth() const {
    int32 width;
    glfwGetFramebufferSize(m_window, &width, nullptr);
    return width;
}

int32 Window::framebufferHeight() const {
    int32 height;
    glfwGetFramebufferSize(m_window, nullptr, &height);
    return height;
}

void Window::setSize(ivec2 extent) {
    glfwSetWindowSize(m_window, extent.x, extent.y);
}

void Window::setSize(int32 width, int32 height) {
    glfwSetWindowSize(m_window, width, height);
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

void Window::update() {
    glfwPollEvents();
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

} // namespace R3
