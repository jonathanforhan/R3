#if not _WIN32
#error "Only Windows support currently"
#endif

#include "Application.hxx"

int main() {
    R3::Application app;
    return app.run();
}