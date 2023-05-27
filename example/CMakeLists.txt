add_executable(cat cat.cpp)
target_link_libraries(cat PRIVATE ione)

add_executable(liburing_cat liburing_cat.cpp)
target_link_libraries(liburing_cat PRIVATE liburing)

add_executable(cp cp.cpp)
target_link_libraries(cp PRIVATE ione)

add_executable(liburing_cp liburing_cp.cpp)
target_link_libraries(liburing_cp PRIVATE liburing)
