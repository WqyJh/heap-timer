# Copyright 2021 Qiying Wang. All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

# using the following command to install benchmark library
# apt install libbenchmark-dev
find_package(benchmark REQUIRED)

set(CMAKE_BUILD_TYPE "Debug")

add_executable(heap_timer_bench
    heap_timer_bench.cc)

target_link_libraries(heap_timer_bench
    benchmark::benchmark
    heaptimer
)

add_custom_target(bench
    COMMAND ./heap_timer_bench
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} # build/bench/
)
