cmake_minimum_required(VERSION 3.13)

if (NOT IONE_LINK_OPTIONS AND NOT IONE_COMPILE_OPTIONS)
  find_package(GTest QUIET)
endif ()

if (NOT GTEST_FOUND)
  include(FetchContent)
  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
    )
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)   # May be enabled later
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif ()

set(TEST_UTIL_INCLUDES
  )
set(TEST_UTIL_SOURCES
  )

set(IONE_UNIT_SOURCES
  unit/context/context
  )

set(IONE_TEST_SOURCES ${IONE_UNIT_SOURCES})

set(OLD_IONE_DEFINITIONS IONE_DEFINITIONS)

if (NOT IONE_DEFINITIONS MATCHES "IONE_CI_SLOWDOWN")
  list(APPEND IONE_DEFINITIONS "IONE_CI_SLOWDOWN=1")
endif ()

function(ione_add_test TEST_NAME)
  target_compile_options(${TEST_NAME} PRIVATE ${IONE_WARN})
  target_compile_definitions(${TEST_NAME} PRIVATE ${IONE_DEFINITIONS})
  target_sources(${TEST_NAME}
    PRIVATE ${TEST_UTIL_INCLUDES}
    )
  target_link_libraries(${TEST_NAME}
    PRIVATE GTest::gtest
    PRIVATE ione
    )
  target_include_directories(${TEST_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${IONE_SOURCE_DIR}/src
    )
  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

if (WARN IN_LIST IONE_FLAGS)
  include(ione_warn_test)
  message("IOneRing/test warnings: ${IONE_WARN}")
endif ()

if (IONE_TEST STREQUAL "SINGLE")
  list(TRANSFORM IONE_TEST_SOURCES APPEND .cpp)
  add_executable(ione_test test.cpp ${IONE_TEST_SOURCES})
  ione_add_test(ione_test)
else ()
  foreach (TEST_SOURCE ${IONE_TEST_SOURCES})
    string(REPLACE "/" "_" TEST_NAME ${TEST_SOURCE})
    add_executable(${TEST_NAME} test.cpp ${TEST_SOURCE}.cpp)
    ione_add_test(${TEST_NAME})
  endforeach ()
endif ()

unset(IONE_TEST_SOURCES)

# TODO: add example flag
add_subdirectory(example)