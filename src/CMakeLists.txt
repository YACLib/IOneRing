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

add_subdirectory(sum)

configure_file(config.hpp.in ${IONE_BINARY_DIR}/include/ione/config.hpp)

add_library(ione STATIC)

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
