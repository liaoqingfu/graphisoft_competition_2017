project(GeneticLearning)
file (GLOB sources "*.cpp")
add_library(GeneticLearning ${sources})
target_link_libraries(GeneticLearning NeuralNetwork)
target_include_directories(GeneticLearning PUBLIC .)
