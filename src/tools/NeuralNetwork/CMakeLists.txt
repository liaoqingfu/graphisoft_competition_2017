project(NeuralNetwork)
file (GLOB sources "*.cpp")
add_library(NeuralNetwork ${sources})
target_include_directories(NeuralNetwork PUBLIC .)
