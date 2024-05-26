# Periodic Timer, a Header-Only C++ 20 Asynchronous Periodic Timer

#### Unit Test and Documentation Generation Workflow Status

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/periodic-timer/build_run_unit_test_cmake.yml?branch=main&label=GH%20Actions%20build,%20unit%20tests%20on%20main)

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/periodic-timer/build_run_unit_test_cmake.yml?branch=develop&label=GH%20Actions%20build,%20unit%20tests%20on%20develop)

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/periodic-timer/gen_docs.yml?branch=main&label=GH%20Actions%20generate%20docs)

![GH Tag](https://img.shields.io/github/v/tag/connectivecpp/periodic-timer?label=GH%20tag)

## Overview

`periodic_timer` is an asynchronous periodic timer that wraps and simplifies Asio timers when periodic callbacks are needed. The periodicity can be based on either a simple duration or on timepoints based on a duration.

Timepoint calculations are performed by this class template so that timepoint durations don't "drift". In other words, if the processing during a callback takes 15 milliseconds, the next callback invocation is adjusted accordingly.

Asynchronous timers from Asio are relatively easy to use. However, there are no timers that are periodic. This class simplifies the usage, using application supplied function object callbacks. When the timer is started, the application specifies whether each callback is invoked based on a duration (e.g. one second after the last callback), or on timepoints (e.g. a callback will be invoked each second according to the clock).

## Generated Documentation

The generated Doxygen documentation for `periodic_timer` is [here](https://connectivecpp.github.io/periodic-timer/).

## Dependencies

The `periodic_timer` header file has the stand-alone Asio library for a dependency. Specific version (or branch) specs for the Asio dependency is in `cmake/download_asio_cpm.cmake`.

## C++ Standard

`periodic_timer` is built under C++ 20, but (currently) does not use any specific C++ 20 features. In the future `concepts` / `requires` will be added.

## Supported Compilers

Continuous integration workflows build and unit test on g++ (through Ubuntu), MSVC (through Windows), and clang (through macOS).

## Unit Test Dependencies

The unit test code uses [Catch2](https://github.com/catchorg/Catch2). If the `PERIODIC_TIMER_BUILD_TESTS` flag is provided to Cmake (see commands below) the Cmake configure / generate will download the Catch2 library as appropriate using the [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) dependency manager. If Catch2 (v3 or greater) is already installed using a different package manager (such as Conan or vcpkg), the `CPM_USE_LOCAL_PACKAGES` variable can be set which results in `find_package` being attempted. Note that v3 (or later) of Catch2 is required.

Specific version (or branch) specs for the Catch2 dependency is in `test/CMakeLists.txt`.

## Build and Run Unit Tests

To build and run the unit test program:

First clone the `periodic-timer` repository, then create a build directory in parallel to the `periodic-timer` directory (this is called "out of source" builds, which is recommended), then `cd` (change directory) into the build directory. The CMake commands:

```
cmake -D PERIODIC_TIMER_BUILD_TESTS:BOOL=ON ../periodic-timer

cmake --build .

ctest
```

For additional test output, run the unit test individually, for example:

```
test/periodic_timer_test -s
```

The example can be built by adding `-D PERIODIC_TIMER_BUILD_EXAMPLES:BOOL=ON` to the CMake configure / generate step.

