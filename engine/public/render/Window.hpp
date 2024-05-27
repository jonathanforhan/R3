/*******************************************************************************
 * @file Window.hpp
 * @brief Cross platform Window class
 ******************************************************************************/

#pragma once

#include "api/Types.hpp"

extern "C" struct GLFWwindow; //!< Opaque pointer needed for Window internals

namespace R3 {

extern class Window* window; //!< Global Window, lasts duration of program

class Window {
private:
    Window();

public:
    Window(const Window&) = delete;
    Window(Window&&)      = delete;
    ~Window();

    /***************************************************************************
     * @brief Called once by engine at the start of the program
     **************************************************************************/
    static void initialize();

private:
    GLFWwindow* m_window;
};

} // namespace R3
