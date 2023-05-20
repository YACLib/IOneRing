macro(add_files)
    set(IONE_INCLUDES ${IONE_INCLUDES} PARENT_SCOPE)
    set(IONE_HEADERS ${IONE_HEADERS} PARENT_SCOPE)
    set(IONE_SOURCES ${IONE_SOURCES} PARENT_SCOPE)
endmacro()

set(IONE_INCLUDE_DIR ${IONE_SOURCE_DIR}/include/ione)
set(IONE_STD_INCLUDE_DIR ${IONE_SOURCE_DIR}/include/ione_std)

set(IONE_INCLUDES
        ${IONE_BINARY_DIR}/include/ione/config.hpp
        )
set(IONE_HEADERS
        )
set(IONE_SOURCES
        )

add_subdirectory(context)

configure_file(config.hpp.in ${IONE_BINARY_DIR}/include/ione/config.hpp)

add_library(ione STATIC)

# yaclib
if (NOT TARGET yaclib)
        include(FetchContent)
        FetchContent_Declare(yaclib
                GIT_REPOSITORY https://github.com/YACLib/YACLib.git
                GIT_TAG main
        )
        set(YACLIB_FLAGS "CORO")
        FetchContent_MakeAvailable(yaclib)
endif()
target_link_libraries(ione PUBLIC yaclib)


# liburing based on this
# https://github.com/questdb/questdb/blob/master/core/CMakeLists.txt#L186
if (UNIX AND NOT APPLE AND CMAKE_SYSTEM_NAME MATCHES "Linux")
    include(ExternalProject)
    ExternalProject_Add(
            liburing_git
            GIT_REPOSITORY http://github.com/axboe/liburing.git
            GIT_TAG liburing-2.2
            BUILD_IN_SOURCE 1
            BUILD_BYPRODUCTS "<SOURCE_DIR>/src/liburing.a"
            BUILD_COMMAND make "CC=${CMAKE_C_COMPILER}" "CXX=${CMAKE_CXX_COMPILER}" "AR=${CMAKE_AR}" "RANLIB=${CMAKE_RANLIB}"
            CONFIGURE_COMMAND ""
            INSTALL_COMMAND ""
            TEST_COMMAND ""
            LOG_BUILD ON
    )
    ExternalProject_Get_Property(liburing_git SOURCE_DIR)
    add_library(liburing INTERFACE)
    add_dependencies(liburing liburing_git)
    target_include_directories(liburing INTERFACE ${SOURCE_DIR}/src/include)
    target_link_libraries(liburing INTERFACE ${SOURCE_DIR}/src/liburing.a)
    target_link_libraries(ione PUBLIC liburing)
endif ()

target_sources(ione
        PUBLIC ${IONE_INCLUDES}
        PRIVATE ${IONE_HEADERS}
        PRIVATE ${IONE_SOURCES}
        )

if (WARN IN_LIST IONE_FLAGS)
    include(ione_warn_lib)
    message("IOneRing/library warnings: ${IONE_WARN}")
endif ()
target_compile_options(ione PRIVATE ${IONE_COMPILE_OPTIONS} ${IONE_WARN})
target_compile_definitions(ione PRIVATE ${IONE_DEFINITIONS})

find_package(Threads REQUIRED)
target_link_libraries(ione PUBLIC Threads::Threads)

target_include_directories(ione
        PUBLIC ${IONE_SOURCE_DIR}/include
        PUBLIC ${IONE_BINARY_DIR}/include # for config.hpp
        PRIVATE ${IONE_SOURCE_DIR}/src
        )

if (IONE_INSTALL)
    install(TARGETS ione)
endif ()
