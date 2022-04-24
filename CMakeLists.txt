cmake_minimum_required(VERSION 2.8)

project(heap-timer)

#add_definitions(-std=c++17)

# generating build/compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-march=native)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
set(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")

set(PUBLIC_HEADERS
    heap_timer.h
)

include_directories(${PROJECT_SOURCE_DIR})
set(SRCS
    heap_timer.c
)

add_library(heaptimer SHARED ${SRCS})
set_target_properties(heaptimer PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
set_target_properties(heaptimer PROPERTIES VERSION 1.0 SOVERSION 1)

add_library(heaptimer_static STATIC ${SRCS})
set_target_properties(heaptimer_static PROPERTIES OUTPUT_NAME "heaptimer")
set_target_properties(heaptimer_static PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")


include(GNUInstallDirs)
install(TARGETS heaptimer heaptimer_static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# -DENABLE_TESTING=ON
option(ENABLE_TESTING "Enable make test" ON)
if (ENABLE_TESTING)
    # apt install libgtest-dev
    enable_testing()
    add_subdirectory(tests)
endif()

# -DENABLE_BENCH=ON
option(ENABLE_BENCH "Enable make bench" ON)
if (ENABLE_BENCH)
    # using the following command to install benchmark library
    # apt install libbenchmark-dev
    add_subdirectory(bench)
endif()
