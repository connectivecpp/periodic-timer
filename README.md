# Periodic Timer, a Header-Only C++ 20 Asynchronous Periodic Timer

#### Unit Test and Documentation Generation Workflow Status

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/periodic-timer/build_run_unit_test_cmake.yml?branch=main&label=GH%20Actions%20build,%20unit%20tests%20on%20main)

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/periodic-timer/build_run_unit_test_cmake.yml?branch=develop&label=GH%20Actions%20build,%20unit%20tests%20on%20develop)

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/periodic-timer/gen_docs.yml?branch=main&label=GH%20Actions%20generate%20docs)

## Overview

`periodic_timer` is an asynchronous periodic timer that wraps and simplifies Asio timers (see [References](https://connectivecpp.github.io/doc/references.html)) when periodic callbacks are needed. The periodicity can be based on either a simple duration or on timepoints based on a duration.

Asynchronous timers from Asio are relatively easy to use. However, there are no timers that are periodic. This class simplifies the usage, using application supplied function object callbacks. When the timer is started, the application specifies whether each callback is invoked based on a duration (e.g. one second after the last callback), or on timepoints (e.g. a callback will be invoked each second according to the clock).

An asynchronous periodic timer simplifying Asio timers, with function object timer callbacks

## Generated Documentation

The generated Doxygen documentation for `periodic_timer` is [here](https://connectivecpp.github.io/periodic-timer/).

## Dependencies

The `periodic_timer` header file has the stand-alone Asio library for a dependency. Specific version (or branch) specs for the Asio dependency is in `test/CMakeLists.txt`.

## C++ Standard

`periodic_timer`  uses C++ 20 features, including `std::stop_token`, `std::stop_source`, `std::condition_variable_any`, `std::scoped_lock`, and `concepts` / `requires`.

## Supported Compilers

Continuous integration workflows build and unit test on g++ (through Ubuntu) and MSVC (through Windows). Note that clang support for C++ 20 `std::jthread` and `std::stop_token` is still experimental (and possibly incomplete) as of May 2024, so has not (yet) been tested with `periodic_timer`.

## Unit Test Dependencies

The unit test code uses [Catch2](https://github.com/catchorg/Catch2). If the `WAIT_QUEUE_BUILD_TESTS` flag is provided to Cmake (see commands below) the Cmake configure / generate will download the Catch2 library as appropriate using the [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) dependency manager. If Catch2 (v3 or greater) is already installed using a different package manager (such as Conan or vcpkg), the `CPM_USE_LOCAL_PACKAGES` variable can be set which results in `find_package` being attempted. Note that v3 (or later) of Catch2 is required.

The unit test uses two third-party libraries (each is a single header-only file):

- Martin Moene's [ring-span-lite](https://github.com/martinmoene/ring-span-lite)
- Justas Masiulis' [circular_buffer](https://github.com/JustasMasiulis/circular_buffer)

Specific version (or branch) specs for the dependencies are in `test/CMakeLists.txt`.

## Build and Run Unit Tests

To build and run the unit test program:

First clone the `periodic-timer` repository, then create a build directory in parallel to the `wait-queue` directory (this is called "out of source" builds, which is recommended), then `cd` (change directory) into the build directory. The CMake commands:

```
cmake -D WAIT_QUEUE_BUILD_TESTS:BOOL=ON -D JM_CIRCULAR_BUFFER_BUILD_TESTS:BOOL=OFF ../wait-queue

cmake --build .

ctest
```

For additional test output, run the unit test individually, for example:

```
test/periodic_timer_test -s
```

The example can be built by adding `-D WAIT_QUEUE_BUILD_EXAMPLES:BOOL=ON` to the CMake configure / generate step.

