if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  list(APPEND IONE_COMPILE_OPTIONS
    /fsanitize=address
    /fsanitize-address-use-after-return
    )
else ()
  if (NOT CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    list(APPEND IONE_LINK_OPTIONS -fsanitize=address)
    list(APPEND IONE_COMPILE_OPTIONS -fno-common)
  endif ()
  list(APPEND IONE_COMPILE_OPTIONS -fsanitize=address)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # -fsanitize=pointer-subtract removed because detect_invalid_pointer_pairs fails with fiber fault injection
    # on test initialization, probably caused by GTest
    # TODO(kononovk) add -fsanitize=pointer-compare when will be fixed:
    # https://github.com/google/sanitizers/issues/1324 also see mentioned issue
    list(APPEND IONE_COMPILE_OPTIONS
      # -fsanitize=pointer-subtract
      -fsanitize-address-use-after-scope
      )
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    list(APPEND IONE_COMPILE_OPTIONS
      # -fsanitize=pointer-compare,pointer-subtract
      -fsanitize-address-use-after-scope
      -mllvm -asan-use-private-alias=1
      )
  endif ()
endif ()
