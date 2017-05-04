project(NeuralClient)

find_package (Threads)
add_executable(neural_client main.cpp)
target_link_libraries(neural_client GeneticLearning client)
target_link_libraries(neural_client ${Boost_LIBRARIES})
target_link_libraries(neural_client ${CMAKE_THREAD_LIBS_INIT})

add_executable(learner learner.cpp)
target_link_libraries(learner GeneticLearning game client cpp-util)
target_link_libraries(learner ${Boost_LIBRARIES})
target_link_libraries(learner ${CMAKE_THREAD_LIBS_INIT})
