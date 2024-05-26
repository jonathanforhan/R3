#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include "Application.hpp"
#include "Engine.hpp"

#if defined WIN32 and (R3_BUILD_DISTRIBUTION == 1)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR lpCmdLine, int mCmdShow) {
#else
int main(int argc, char* argv[]) {
#endif
    Application app();
    Engine engine();

    std::cout << "Hello World\n";

    return 0;
}