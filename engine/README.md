# R3 Engine

## File Structure

### public

- Contains the public API for R3 Engine, these headers are publicly included and accessible from client Application
- Denoted with `.cpp`, and `.hpp` file extension

### private

- Contains the implementation of the API, these headers are private and not accessible from client API
- Denoted with `.cxx` and `.hxx` file extension

### extern

- Contains external dependencies, equivalent to `vendor` in other projects

## Modules

R3 uses the concept of *Modules* which in reality are just namespaces but break up the project into more modular pieces

If you use a module namespace (for example glTF, opengl, vulkan, etc) that module name is included in the filename for example `foomodule-barsubmodule-baz.cxx`, I've seen that project who don't do this and have many of the same named file are excedingly confusing

**NOTE** In the *Hardware Abstraction Layer* (hal) we add a module specifier to the renderer-specific files even if they do not have a specific namespace (this is due to have a public facing header that is used for all renderer implementations)

## Conditional Compilation	

Rather than doing this CMake we simply put guards like `#if R3_OPENGL` to only compile the code if the `R3_OPENGL` option is set, this is more straight-forward than doing some CMakeLists logic
