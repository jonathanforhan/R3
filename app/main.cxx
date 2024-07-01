#if defined WIN32 and R3_BUILD_DISTRIBUTION
#include <windows.h>
#endif

#include <api/Log.hpp>
#include <api/Result.hpp>
#include "Application.hxx"

#if defined WIN32 and R3_BUILD_DISTRIBUTION
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR lpCmdLine, int mCmdShow) {
#else
int main() {
#endif
    R3::Application app;

    auto result = app.run();

    if (!result) {
        R3_LOG(Error, "application returned status code {}", (int)result.error());
        return -1;
    }
    return 0;
}