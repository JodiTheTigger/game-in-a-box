#    Game-in-a-box. Simple First Person Shooter Network Game.
#    Copyright (C) 2012 Richard Maxwell <jodi.the.tigger@gmail.com>
#    
#    This file is part of Game-in-a-box
#
#    Game-in-a-box is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.
#
#    You should have received a copy of the GNU Affero General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(game-in-a-box CXX)

###############
# Prerequisites 
###############

# set gcc to use c++0x11
if(CMAKE_COMPILER_IS_GNUCXX)

  # Make sure we have g++ 4.7 or higher for c++11 features
  # annoyingly in cmake 2.8.8 this is much easier, but I don't have
  # that in ubuntu 12.04
  exec_program(
      ${CMAKE_CXX_COMPILER}
      ARGS                    --version
      OUTPUT_VARIABLE _compiler_output)
  string(REGEX REPLACE ".* ([0-9]\\.[0-9]\\.[0-9]).*" "\\1"
         gcc_compiler_version ${_compiler_output})
  message(STATUS "C++ compiler version: '${gcc_compiler_version}' [${CMAKE_CXX_COMPILER}]")

  if(NOT gcc_compiler_version MATCHES "[0-9]\\.[0-9]\\.[0-9]")
    message(FATAL_ERROR "Cannot deduce C++ version information.")
  endif()

  if(NOT gcc_compiler_version VERSION_GREATER "4.6.99")
    message(FATAL_ERROR "Requires GCC version 4.7 or greater for C++11 functionality. Got ${gcc_compiler_version}.")
  endif()

  set(CMAKE_CXX_FLAGS "-std=c++0x")                                                                                                                                                                                                                         
endif()

###############
# Server 
###############    
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(
game-in-a-box-server 
common/ReflectedAgain.cpp 
common/IReflected.cpp 
common/ReflectionManager.cpp 
common/StateManager.cpp 
common/NetworkManager.cpp 
common/NetworkProvider.cpp 
common/BitStream.cpp 
common/RollingStatistics.cpp
server/GameInABoxServer.cpp 
server/main.cpp)

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
  ${PROJECT_SOURCE_DIR}/common/ReflectionManager.cpp
  ${PROJECT_SOURCE_DIR}/common/IReflected.cpp 
  ${PROJECT_SOURCE_DIR}/common/TestReflectionManager.cpp 
  ${PROJECT_SOURCE_DIR}/common/TestIReflected.cpp 
  ${PROJECT_SOURCE_DIR}/common/RollingStatistics.cpp 
  ${PROJECT_SOURCE_DIR}/common/TestRollingStatistics.cpp 
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




