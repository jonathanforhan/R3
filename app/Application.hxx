#pragma once

namespace R3 {

/*******************************************************************************
 * @brief R3 Application class that drive program
 *
 * Initializes and deinitializes static globals like Window class and runs the
 * Engine
 ******************************************************************************/
class Application {
public:
    /*******************************************************************************
     * @brief Main driver for engine, kicks off application
     * @return Exit status
     ******************************************************************************/
    int run();
};

} // namespace R3