cmake_minimum_required(VERSION 3.10)

project(ione
        LANGUAGES CXX
        )

set(IONE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(IONE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    add_compile_options(-fcoroutines)
endif ()

list(APPEND YACLIB_COMPILE_OPTIONS "${YACLIB_CORO_FLAGS}")


# Include guards
if (IONE_SOURCE_DIR STREQUAL IONE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${IONE_SOURCE_DIR}/cmake")
include(ione_flags)

if (IONE_INSTALL)
    include(GNUInstallDirs)
    install(DIRECTORY ${IONE_SOURCE_DIR}/include/ione TYPE INCLUDE)
    install(DIRECTORY ${IONE_BINARY_DIR}/include/ione TYPE INCLUDE)
endif()

add_link_options(${IONE_LINK_OPTIONS})
add_compile_options(${IONE_COMPILE_OPTIONS})

add_subdirectory(src) # Create static library

if (IONE_TEST)
    enable_testing()
    add_subdirectory(test)
    message("CMAKE_CXX_COMPILER_ID : ${CMAKE_CXX_COMPILER_ID}")
    message("CMAKE_CXX_SIMULATE_ID : ${CMAKE_CXX_SIMULATE_ID}")
    message("IONE_LINK_OPTIONS   : ${IONE_LINK_OPTIONS}")
    message("IONE_COMPILE_OPTIONS: ${IONE_COMPILE_OPTIONS}")
    message("IONE_DEFINITIONS    : ${IONE_DEFINITIONS}")
endif ()
