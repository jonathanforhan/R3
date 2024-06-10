#if defined WIN32 and R3_BUILD_DISTRIBUTION
#include <windows.h>
#endif

#include "Application.hxx"
#include "api/Log.hpp"

#if defined WIN32 and R3_BUILD_DISTRIBUTION
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR lpCmdLine, int mCmdShow) {
#else
int main(int argc, char* argv[]) {
#endif
    R3::Application app;
    int ret;

    if ((ret = app.run()) != 0) {
        LOG(Error, "application returned status code {}", ret);
    }
    return ret;
}