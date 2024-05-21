# Copyright (c) 2024 by Cliff Green
#
# https://github.com/connectivecpp/periodic-timer
#
# I'm still learning CMake, so improvement suggestions are always welcome.
#
# The Asio CMake code is taken from CPM.cmake examples/asio-standalone.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

project ( periodic_timer 
	  LANGUAGES CXX 
	  DESCRIPTION "An asynchronous periodic timer based on Asio"
	  HOMEPAGE_URL "https://github.com/connectivecpp/periodic-timer/" )

option ( PERIODIC_TIMER_BUILD_TESTS "Build unit tests" OFF )
option ( PERIODIC_TIMER_BUILD_EXAMPLES "Build examples" OFF )
option ( PERIODIC_TIMER_INSTALL "Install header only library" OFF )

# add library targets

add_library ( periodic_timer INTERFACE )
add_library ( chops::periodic_timer ALIAS periodic_timer )

# thread support specified in Asio download

include ( cmake/download_asio_cpm.cmake )

# configure library target

target_include_directories ( periodic_timer INTERFACE
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
			     $<INSTALL_INTERFACE:include/> )
target_compile_features ( periodic_timer INTERFACE cxx_std_20 )

# check to build unit tests
if ( ${PERIODIC_TIMER_BUILD_TESTS} )
  enable_testing()
  add_subdirectory ( test )
endif ()

# check to build example code
if ( ${PERIODIC_TIMER_BUILD_EXAMPLES} )
  add_subdirectory ( example )
endif ()

# check to install
if ( ${PERIODIC_TIMER_INSTALL} )
  set ( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt )
  include ( CPack )
endif ()

# end of file

