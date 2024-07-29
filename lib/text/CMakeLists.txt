add_library(text STATIC)

target_include_directories(text PUBLIC ..)
target_sources(text PRIVATE string.c string_array.c)
