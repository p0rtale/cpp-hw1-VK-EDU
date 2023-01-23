project(ustring_app)

add_executable(ustring main.cpp)
target_link_libraries(ustring PRIVATE ustring_lib)
