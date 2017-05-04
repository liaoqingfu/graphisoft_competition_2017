project(Serverless)
file (GLOB sources "*.cpp")

find_package (Threads)
add_executable(serverless ${sources})
target_link_libraries(serverless client game)
target_link_libraries(serverless ${Boost_LIBRARIES})
target_link_libraries(serverless ${CMAKE_THREAD_LIBS_INIT})
