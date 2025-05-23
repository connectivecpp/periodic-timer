# Copyright (c) 2024-2025 by Cliff Green
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

# create project
project ( periodic_timer_example LANGUAGES CXX )

# add executable
add_executable ( periodic_timer_example periodic_timer_example.cpp )
target_compile_features ( periodic_timer_example PRIVATE cxx_std_20 )

set ( CMAKE_THREAD_PREFER_PTHREAD TRUE )
set ( THREADS_PREFER_PTHREAD_FLAG TRUE )
find_package ( Threads REQUIRED )

# link dependencies
target_link_libraries ( periodic_timer_example PRIVATE 
	Threads::Threads asio periodic_timer )

