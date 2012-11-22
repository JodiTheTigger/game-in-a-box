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
  set(CMAKE_CXX_FLAGS "-std=c++0x")                                                                                                                                                                                                                         
endif()

###############
# Server 
###############    
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(
game-in-a-box-server 
common/StateManager.cpp 
common/NetworkManager.cpp 
common/NetworkProvider.cpp 
server/GameInABoxServer.cpp 
common/BitStream.cpp 
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




