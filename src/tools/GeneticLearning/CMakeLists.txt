project(GeneticLearning)
file (GLOB sources "*.cpp")
include_directories(${NeuralNetwork_SOURCE_DIR})
add_library(GeneticLearning ${sources})
