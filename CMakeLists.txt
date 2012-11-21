cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(game-in-a-box CXX)

###############
# Prerequisites 
###############

# set gcc to use c++0x11
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x")                                                                                                                                                                                                                         
endif()

###############
# Server 
###############    
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(game-in-a-box-server common/RollingStatistics.cpp common/BitStream.cpp server/main.cpp)
target_link_libraries(game-in-a-box-server) 
add_custom_command(TARGET game-in-a-box-server POST_BUILD COMMAND game-in-a-box-common-tests)
    
###############
# Client 
###############

###############
# Testing 
###############
# yikes! enable google test. Since this doesn't have a module, do it manually.
# http://stackoverflow.com/questions/9689183/cmake-googletest/9695234#9695234
# yikes! that wasted a day of my life. Doesn't work as it can't find a make target
# Maybe it only works in windows? Either way, include gmock with the source.

add_subdirectory(third-party/gmock-1.6.0/gtest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

ADD_EXECUTABLE(
  game-in-a-box-common-tests 
  ${PROJECT_SOURCE_DIR}/common/BitStream.cpp
  ${PROJECT_SOURCE_DIR}/common/TestBitStream.cpp)
  
target_link_libraries(
  game-in-a-box-common-tests
  pthread
  gtest
  gtest_main)
  
add_test(
  NAME game-in-a-box-common-tests
  COMMAND game-in-a-box-common-tests
)




