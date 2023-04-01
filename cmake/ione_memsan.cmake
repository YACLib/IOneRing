# Memory Sanitizer doesn't work without compile stdlib with it
list(APPEND IONE_LINK_OPTIONS -fsanitize=memory)
list(APPEND IONE_COMPILE_OPTIONS -fsanitize=memory)
