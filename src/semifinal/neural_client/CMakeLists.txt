project(NeuralClient)

# TODO remove
# and use only target_link_libraries for cpp-util
include_directories(${cpp-util_SOURCE_DIR}/include)

add_executable(neural_client main.cpp)
target_link_libraries(neural_client GeneticLearning client)
target_link_libraries(neural_client ${Boost_LIBRARIES})

add_executable(learner learner.cpp)
target_link_libraries(learner GeneticLearning game client cpp-util)
target_link_libraries(learner ${Boost_LIBRARIES})
