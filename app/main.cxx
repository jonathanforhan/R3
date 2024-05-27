#ifdef WIN32
#include <windows.h>
#endif

#include "Application.hxx"
#include "render/Window.hpp"

#if defined WIN32 and R3_BUILD_DISTRIBUTION
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR lpCmdLine, int mCmdShow) {
#else
int main(int argc, char* argv[]) {
#endif

    R3::Window::initialize();

    return 0;
}