add_executable(cat cat.cpp)
target_link_libraries(cat PRIVATE ione)

add_executable(liburing_cat liburing_cat.cpp)
target_link_libraries(liburing_cat PRIVATE liburing)