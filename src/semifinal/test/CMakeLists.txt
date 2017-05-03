file (GLOB sources *.cpp)
add_executable(semifinal_test ${sources})
target_link_libraries(semifinal_test client)
target_link_libraries(semifinal_test ${Boost_LIBRARIES})
