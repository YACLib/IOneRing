if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # TODO(kononovk) We have to use /Wall. We need to disable some paranoid warnings, but I don't have windows.
  list(APPEND IONE_WARN
    /W3
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  list(APPEND IONE_WARN
    -Weverything
    -Wno-c++98-compat
    -Wno-c++98-compat-pedantic
    -Wno-ctad-maybe-unsupported
    -Wno-padded
    -Wno-exit-time-destructors
    -Wno-undefined-func-template # Needed for some hacks that speed up compilation
    -Wno-weak-template-vtables   # TODO(kononovk) Maybe fix it
    -Wno-switch-enum             # TODO(kononovk) Maybe fix it
    -Wno-weak-vtables            # TODO(kononovk) Fix it
    -Wno-global-constructors   # TODO(kononovk) Maybe fix it: Needed only for tests
    -Wno-sign-conversion       # TODO(kononovk) Maybe fix it: Needed only for tests
    -Wno-gnu-zero-variadic-macro-arguments  # TODO(kononovk) Fix it: Needed only for tests
    -Wno-covered-switch-default  # TODO(kononovk) Fix it: test/unit/algo/when_any.cpp
    -Wno-shadow-uncaptured-local # TODO(kononovk) Fix it: test/unit/algo/when_any.cpp
    )
  if (CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    list(APPEND IONE_WARN
      -Wno-undef # For gtest
      )
  endif ()
else ()
  list(APPEND IONE_WARN
    -Wall
    -Wextra
    -pedantic
    )
endif ()
