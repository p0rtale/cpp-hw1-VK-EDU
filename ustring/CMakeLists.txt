get_filename_component(LIB_INCLUDE_PATH "." ABSOLUTE)

add_library(ustring_lib STATIC ustring.cpp)
target_include_directories(ustring_lib PUBLIC ${LIB_INCLUDE_PATH})
