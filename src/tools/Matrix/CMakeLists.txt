project(Matrix)
file (GLOB sources "*.cpp")
add_library(Matrix ${sources})
target_include_directories(Matrix PUBLIC .)
