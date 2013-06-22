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
project(game-in-a-box C CXX)

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
check_cxx_compiler_flag("-std=c++11 -stdlib=libc++" HAS_CPP11_AND_LIB)
check_cxx_compiler_flag(-Weffc++ HAS_WEFFICENTCPLUSPLUS)
check_cxx_compiler_flag(-Wall HAS_WALL)
check_cxx_compiler_flag(-W4 HAS_W4)
check_cxx_compiler_flag(-Wextra HAS_WEXTRA)
check_cxx_compiler_flag(-Weffc++ HAS_WEFFICENTCPLUSPLUS)
check_cxx_compiler_flag(-Werror HAS_WERROR)
check_cxx_compiler_flag(-Wno-non-virtual-dtor HAS_IGNORE_NONVIRTUAL_DTOR)

# OK, so MSVC12 is needed for c++11 but doesn't support it without the
# November 2012 CTP as the platform tool-set.
# CMAKE doesn't support specifying the tool-set yet, so the user has to
# Manually do it!
# http://blogs.msdn.com/b/vcblog/archive/2012/11/02/visual-c-c-11-and-the-future-of-c.aspx
# Manually do it!
# ARGH! MSVC12 is actually MSVC11!
if (MSVC11)
	# We require cmake "-T" option to support C++11 platform in VS2012
	# This was added in CMAKE 2.8.11. This has to be set by the user, not us.
	cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
	
	if(NOT CMAKE_GENERATOR_TOOLSET MATCHES "v120_CTP_Nov2012")
		message(FATAL_ERROR "MSVC needs C++11 features, please run cmake with the command parameter '-T v120_CTP_Nov2012' or configure the cache entry CMAKE_GENERATOR_TOOLSET to 'v120_CTP_Nov2012'.")
	endif()	
	
	set(HAS_MSCPP true)
	
	# gtest and gmock need 10 deep VARIADIC template functions.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_VARIADIC_MAX=10")
endif()

# apply compiler options
if (HAS_CPP11_AND_LIB)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
elseif (HAS_CPP11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(HAS_CPP0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif(NOT MSVC11)
    message(FATAL_ERROR "Compiler requires C++11 functionality.")
endif()

# prefer W4 over WALL for MSVC at least.
if (HAS_W4)
    set(PARANOID_FLAGS "${PARANOID_FLAGS} -W4")
elseif(HAS_WALL)
    set(PARANOID_FLAGS "${PARANOID_FLAGS} -Wall")
endif()

if (HAS_WEXTRA)
    set(PARANOID_FLAGS "${PARANOID_FLAGS} -Wextra")
endif()

# Can't ignore "Base class has non-virtual destructor warnings :-("
#if (HAS_WEFFICENTCPLUSPLUS)
#    if (HAS_IGNORE_NONVIRTUAL_DTOR)
#        set(PARANOID_FLAGS "${PARANOID_FLAGS} -Weffc++")
#
#        # First warning I'm ignoring
#        # "class X has virtual functions and an accessible non-virtual destructor."
#        # As my NVI interfaces have protected destructors it doesn't matter.
#        # Bug 7302 - -Wnon-virtual-dtor should't complain of protected dtor
#        set(PARANOID_FLAGS "${PARANOID_FLAGS} -Wno-non-virtual-dtor")
#    endif()
#endif()

if (HAS_WERROR)
    set(PARANOID_FLAGS "${PARANOID_FLAGS} -Werror")
endif()

###############
# Third Party Libraries
###############

# TODO! Move to 1.53.0 for c++11 support.
FIND_PACKAGE( Boost 1.46.1 COMPONENTS system thread REQUIRED )
INCLUDE_DIRECTORIES( SYSTEM ${Boost_INCLUDE_DIR} )
# Uses threads in Linuxland
FIND_PACKAGE ( Threads )

###############
# Common Library
###############

set(COMMON_SOURCE
source/Common/No.hpp
source/Common/PrecompiledHeaders.cpp
source/Common/PrecompiledHeaders.hpp
source/Common/AutoComplete.hpp
source/Common/Huffman.hpp
source/Common/MakeUnique.hpp
source/Common/ReflectionKey.hpp
source/Common/WrappingCounter.hpp
source/Common/BitStreamReadOnly.cpp
source/Common/IReflected.cpp
source/Common/IStateManager.cpp
source/Common/ClientHandle.hpp
source/Common/Huffman.cpp
source/Common/RollingStatistics.cpp
source/Common/BitStream.cpp
source/Common/AutoComplete.cpp
source/Common/GameSimple.cpp
source/Common/ReflectionManager.cpp
source/Common/BitStream.hpp
source/Common/GameSimple.hpp
source/Common/IStateManager.hpp
source/Common/ReflectionManager.hpp
source/Common/RollingStatistics.hpp
source/Common/BitStreamReadOnly.hpp
source/Common/IReflected.hpp
source/Common/Sequence.hpp
source/Common/MotleyUniquePointer.hpp
)

set(COMMON_SOURCE_NETWORK
source/Common/Network/Hash.hpp
source/Common/Network/NetworkKey.hpp
source/Common/Network/NetworkKey.cpp
source/Common/Network/Connection.cpp
source/Common/Network/Connection.hpp
source/Common/Network/BufferSerialisation.hpp
source/Common/Network/DeltaMapItem.hpp
source/Common/Network/XorCode.hpp
source/Common/Network/INetworkManager.cpp
source/Common/Network/PacketFragmentManager.hpp
source/Common/Network/PacketFragmentManager.cpp
source/Common/Network/INetworkProvider.cpp
source/Common/Network/Packet.cpp
source/Common/Network/Packets.hpp
source/Common/Network/PacketCommand.hpp
source/Common/Network/PacketCommandWithKey.hpp
source/Common/Network/PacketDelta.cpp
source/Common/Network/PacketFragment.cpp
source/Common/Network/PacketFragment.hpp
source/Common/Network/PacketChallengeResponse.cpp
source/Common/Network/PacketChallenge.cpp
source/Common/Network/PacketDelta.hpp
source/Common/Network/PacketChallenge.hpp
source/Common/Network/Packet.hpp
source/Common/Network/PacketChallengeResponse.hpp
source/Common/Network/NetworkManagerServer.cpp
source/Common/Network/NetworkManagerServer.hpp
source/Common/Network/INetworkManager.hpp
source/Common/Network/INetworkProvider.hpp
source/Common/Network/NetworkManagerClient.hpp
source/Common/Network/DeltaCoder.hpp
source/Common/Network/DeltaCoder-Implementation.hpp
source/Common/Network/NetworkManagerClient.cpp
source/Common/Network/NetworkPacket.hpp
source/Common/Network/NetworkProviderSynchronous.cpp
source/Common/Network/NetworkProviderSynchronous.hpp
source/Common/Network/NetworkProviderInMemory.cpp
source/Common/Network/NetworkProviderInMemory.hpp
)

set(COMMON_SOURCE_LOGGING
source/Common/Logging/Logging.hpp
source/Common/Logging/Logging.cpp
)

###############
# Server 
###############    
# RAM: Debug, get clang to find the bits/c++config.h file!
#include_directories(/usr/include/i386-linux-gnu/c++/4.7/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/)

add_library(
game-in-a-box-common
${COMMON_SOURCE}
${COMMON_SOURCE_NETWORK}
${COMMON_SOURCE_LOGGING}
)

add_executable(
game-in-a-box-server
source/Server/GameInABoxServer.cpp 
source/Server/main.cpp)

set_target_properties(game-in-a-box-server PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS}")
set_target_properties(game-in-a-box-common PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS}")
target_link_libraries(game-in-a-box-server game-in-a-box-common ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
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
include_directories(SYSTEM ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mock/)

ADD_EXECUTABLE(
  game-in-a-box-common-tests
  ${PROJECT_SOURCE_DIR}/mock/Common/Network/MockINetworkProvider.hpp
  ${PROJECT_SOURCE_DIR}/mock/Common/MockIStateManager.hpp
  ${PROJECT_SOURCE_DIR}/test/Common/TestWrappingCounter.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/Network/TestConnection.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/Network/TestPackets.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/Network/TestPacketDelta.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/Network/TestPacketFragment.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/Network/TestPacketFragmentManager.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/Network/TestDeltaCoder.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/Network/TestXorCode.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/Network/TestBufferSerialisation.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/Network/TestNetworkProviderSynchronous.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/Network/TestNetworkManagerClient.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/Network/TestNetworkManagerServer.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/TestHuffman.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/TestReflectionManager.cpp 
  ${PROJECT_SOURCE_DIR}/test/Common/TestIReflected.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/TestRollingStatistics.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/TestBitStreamReadOnly.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/TestBitStream.cpp
  ${PROJECT_SOURCE_DIR}/test/Common/TestAutoComplete.cpp)

set_target_properties(game-in-a-box-common-tests PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS}")

target_link_libraries(
  game-in-a-box-common-tests
  game-in-a-box-common
  ${CMAKE_THREAD_LIBS_INIT}
  gmock
  gmock_main
  ${Boost_LIBRARIES})
  
add_test(
  NAME game-in-a-box-common-tests
  COMMAND game-in-a-box-common-tests
)
