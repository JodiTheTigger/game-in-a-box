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
include(CheckCXXCompilerFlag)
project(game-in-a-box CXX)

###############
# Prerequisites 
###############

# Check features, not compiler.
# Left in here incase there is some compiler based bugs we need
# to check against.
#
# set gcc to use c++0x11
#if(CMAKE_COMPILER_IS_GNUCXX)
#
#  # Make sure we have g++ 4.7 or higher for c++11 features
#  # annoyingly in cmake 2.8.8 this is much easier, but I don't have
#  # that in ubuntu 12.04
#  exec_program(
#      ${CMAKE_CXX_COMPILER}
#      ARGS                    --version
#      OUTPUT_VARIABLE _compiler_output)
#  string(REGEX REPLACE ".* ([0-9]\\.[0-9]\\.[0-9]).*" "\\1"
#         gcc_compiler_version ${_compiler_output})
#  message(STATUS "C++ compiler version: '${gcc_compiler_version}' [${CMAKE_CXX_COMPILER}]")
#
#  if(NOT gcc_compiler_version MATCHES "[0-9]\\.[0-9]\\.[0-9]")
#    message(FATAL_ERROR "Cannot deduce C++ version information.")
#  endif()
#
#  if(NOT gcc_compiler_version VERSION_GREATER "4.6.99")
#    message(FATAL_ERROR "Requires GCC version 4.7 or greater for C++11 functionality. Got ${gcc_compiler_version}.")
#  endif()
#  
#  set(CMAKE_CXX_FLAGS "-std=c++0x")                                                                                                                                                                                                                         
#endif()

# Check compiler options
check_cxx_compiler_flag("-std=c++11" HAS_CPP11)
check_cxx_compiler_flag("-std=c++0x" HAS_CPP0X)
check_cxx_compiler_flag(-Wall HAS_WALL)
check_cxx_compiler_flag(-W4 HAS_W4)
check_cxx_compiler_flag(-Wextra HAS_WEXTRA)
check_cxx_compiler_flag(-Weffc++ HAS_WEFFICENTCPLUSPLUS)
check_cxx_compiler_flag(-Werror HAS_WERROR)

# OK, so MSVC12 is needed for c++11 but doesn't support it without the
# November 2012 CTP as the platform tool-set.
# CMAKE doesn't support specifying the tool-set yet, so the user has to
# Manually do it!
# http://blogs.msdn.com/b/vcblog/archive/2012/11/02/visual-c-c-11-and-the-future-of-c.aspx
if (MSVC12)
	set(HAS_MSCPP true)
	
	# Bah, can't get this to work either. Have to do it manually.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_VARIADIC_MAX=10")
	
	# Maybe this will work someday (doesn't work in 2.8.11)
	set(CMAKE_VS_PLATFORM_TOOLSET "v120_CTP_Nov2012")
endif()

# apply compiler options
if (HAS_CPP11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(HAS_CPP0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif(NOT MSVC11)
    message(FATAL_ERROR "Compiler requires C++11 functionality.")
endif()

#prefer W4 over WALL for MSVC at least.
if (HAS_W4)
    set(PARANOID_FLAGS "${PARANOID_FLAGS} -W4")
elseif(HAS_WALL)
    set(PARANOID_FLAGS "${PARANOID_FLAGS} -Wall")
endif()

if (HAS_WEXTRA)
    set(PARANOID_FLAGS "${PARANOID_FLAGS} -Wextra")
endif()

# C++11 allows in-class initialization of data members
# GCC 4.7 effc++ hasn't been updated to support this yet
#if (HAS_WEFFICENTCPLUSPLUS)
#    set(PARANOID_FLAGS "${PARANOID_FLAGS} -Weffc++")
#endif()

if (HAS_WERROR)
    set(PARANOID_FLAGS "${PARANOID_FLAGS} -Werror")
endif()

###############
# Third Party Libraries
###############

# TODO! Move to 1.52.0 for c++11 support.
FIND_PACKAGE( Boost 1.46.1 COMPONENTS system thread REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

###############
# Server 
###############    
# RAM: Debug, get clang to find the bits/c++config.h file!
include_directories(/usr/include/i386-linux-gnu/c++/4.7/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(
game-in-a-box-server
common/Sequence.cpp
common/NetworkManagerBase.cpp
common/NetworkManagerServer.cpp
common/GameInABox.cpp
common/IStateManager.cpp  
common/Huffman.cpp 
common/BitStreamReadOnly.cpp 
common/BitStream.cpp
common/NetworkCommand.cpp
common/AutoComplete.cpp
common/IReflected.cpp 
common/ReflectionManager.cpp 
common/NetworkManager.cpp 
common/NetworkProvider.cpp 
common/RollingStatistics.cpp
server/GameInABoxServer.cpp 
server/main.cpp)

set_target_properties(game-in-a-box-server PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS}")
target_link_libraries(game-in-a-box-server pthread ${Boost_LIBRARIES}) 
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
add_subdirectory(third-party/gmock-1.6.0)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

ADD_EXECUTABLE(
  game-in-a-box-common-tests 
  ${PROJECT_SOURCE_DIR}/common/GameInABox.cpp
  ${PROJECT_SOURCE_DIR}/common/IStateManager.cpp  
  ${PROJECT_SOURCE_DIR}/common/TestDeltaCoder.cpp
  ${PROJECT_SOURCE_DIR}/common/Huffman.cpp
  ${PROJECT_SOURCE_DIR}/common/TestHuffman.cpp
  ${PROJECT_SOURCE_DIR}/common/NetworkProvider.cpp
  ${PROJECT_SOURCE_DIR}/common/TestNetworkProvider.cpp
  ${PROJECT_SOURCE_DIR}/common/ReflectionManager.cpp
  ${PROJECT_SOURCE_DIR}/common/IReflected.cpp 
  ${PROJECT_SOURCE_DIR}/common/TestReflectionManager.cpp 
  ${PROJECT_SOURCE_DIR}/common/TestIReflected.cpp 
  ${PROJECT_SOURCE_DIR}/common/RollingStatistics.cpp 
  ${PROJECT_SOURCE_DIR}/common/TestRollingStatistics.cpp 
  ${PROJECT_SOURCE_DIR}/common/BitStreamReadOnly.cpp
  ${PROJECT_SOURCE_DIR}/common/TestBitStreamReadOnly.cpp
  ${PROJECT_SOURCE_DIR}/common/BitStream.cpp
  ${PROJECT_SOURCE_DIR}/common/TestBitStream.cpp
  ${PROJECT_SOURCE_DIR}/common/AutoComplete.cpp
  ${PROJECT_SOURCE_DIR}/common/TestAutoComplete.cpp)
  
target_link_libraries(
  game-in-a-box-common-tests
  pthread
  gtest
  gtest_main
  ${Boost_LIBRARIES})
  
add_test(
  NAME game-in-a-box-common-tests
  COMMAND game-in-a-box-common-tests
)
