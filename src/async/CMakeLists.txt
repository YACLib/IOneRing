list(APPEND IONE_INCLUDES
  ${IONE_INCLUDE_DIR}/async/accept.hpp
  ${IONE_INCLUDE_DIR}/async/close.hpp
  ${IONE_INCLUDE_DIR}/async/connect.hpp
  ${IONE_INCLUDE_DIR}/async/fadvise.hpp
  ${IONE_INCLUDE_DIR}/async/fallocate.hpp
  ${IONE_INCLUDE_DIR}/async/fsync.hpp
  ${IONE_INCLUDE_DIR}/async/madvise.hpp
  ${IONE_INCLUDE_DIR}/async/nop.hpp
  ${IONE_INCLUDE_DIR}/async/openat.hpp
  ${IONE_INCLUDE_DIR}/async/openat2.hpp
  ${IONE_INCLUDE_DIR}/async/read.hpp
  ${IONE_INCLUDE_DIR}/async/recv.hpp
  ${IONE_INCLUDE_DIR}/async/send.hpp
  ${IONE_INCLUDE_DIR}/async/splice.hpp
  ${IONE_INCLUDE_DIR}/async/statx.hpp
  ${IONE_INCLUDE_DIR}/async/write.hpp
  ${IONE_INCLUDE_DIR}/async/timeout.hpp
  )

list(APPEND IONE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/all.cpp
  )

add_files()
