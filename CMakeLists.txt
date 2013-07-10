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
check_cxx_compiler_flag("-fno-rtti" CAN_DISABLE_RTTI)
check_cxx_compiler_flag(-Weffc++ HAS_WEFFICENTCPLUSPLUS)
check_cxx_compiler_flag(-Wall HAS_WALL)
check_cxx_compiler_flag(-W4 HAS_W4)
check_cxx_compiler_flag(-Wextra HAS_WEXTRA)
check_cxx_compiler_flag(-Weffc++ HAS_WEFFICENTCPLUSPLUS)
check_cxx_compiler_flag(-Werror HAS_WERROR)
check_cxx_compiler_flag(-Wno-non-virtual-dtor HAS_IGNORE_NONVIRTUAL_DTOR)
check_cxx_compiler_flag(-Wno-unused-local-typedefs HAS_IGNORE_UNUSED_LOCAL_TYPEDEFS)
check_cxx_compiler_flag(-Wno-mismatched-tags HAS_IGNORE_MISMATCHED_TAGS)

# Only platforms with clangs libc++ will build with clang with these build flags.
# Currently, that's only OSX.
check_cxx_source_compiles(CMAKE_REQUIRED_FLAGS="-std=c++11 -stdlib=libc++" "
#include <cstdint>
int main(int, int)
{
    return 0;
}
"
, HAS_CPP11_AND_LIB)

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
if (CAN_DISABLE_RTTI)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -DBOOST_NO_RTTI -DBOOST_NO_TYPEID -DGTEST_HAS_RTTI=0")
endif()

if (HAS_CPP11_AND_LIB)
    message("Using HAS_CPP11_AND_LIB.")
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

# Silence gtest and gmock please
if (HAS_IGNORE_UNUSED_LOCAL_TYPEDEFS)
    set(GMOCK_FLAGS "${GMOCK_FLAGS} -Wno-unused-local-typedefs")
endif()

if (HAS_IGNORE_MISMATCHED_TAGS)
    # Ignore due to implmenting std::hash templates the standard uses
    # both struct hash and class hash. Not much I can do there.
    set(IGNOREWARNINGS_FLAGS "${IGNOREWARNINGS_FLAGS} -Wno-mismatched-tags")
endif()


###############
# Third Party Libraries
###############

# TODO! Move to 1.53.0 for c++11 support.
# Clang needs 1.48 otherwise it doesn't build due to calling a deleted constructor.
FIND_PACKAGE( Boost 1.48 COMPONENTS system thread REQUIRED )
INCLUDE_DIRECTORIES( SYSTEM ${Boost_INCLUDE_DIR} )
# Uses threads in Linuxland
FIND_PACKAGE ( Threads )

###############
# Source Code
###############
# RAM: TODO: Clear up this mess. Library-fy network please.
# Move No.hpp and Units.hpp into a common header directory maybe?
# Maybe BaseClassUniquePointer.hpp too if it's ever used?

set(NETWORK_HEADERS
include/Network/ClientHandle.hpp
include/Network/Delta.hpp
include/Network/INetworkManager.hpp
include/Network/INetworkProvider.hpp
include/Network/IStateManager.hpp
include/Network/NetworkManagerClient.hpp
include/Network/NetworkManagerServer.hpp
include/Network/NetworkPacket.hpp
include/Network/No.hpp
include/Network/Sequence.hpp
include/Network/WrappingCounter.hpp
)

set(NETWORK
source/Network/INetworkManager.cpp
source/Network/INetworkProvider.cpp
source/Network/IStateManager.cpp
source/Network/NetworkManagerClient.cpp
source/Network/NetworkManagerServer.cpp
source/Network/Implementation/BitStream.cpp
source/Network/Implementation/BitStream.hpp
source/Network/Implementation/BitStreamReadOnly.hpp
source/Network/Implementation/BitStreamReadOnly.cpp
source/Network/Implementation/BufferSerialisation.hpp
source/Network/Implementation/Connection.cpp
source/Network/Implementation/Connection.hpp
source/Network/Implementation/Hash.hpp
source/Network/Implementation/Huffman.hpp
source/Network/Implementation/Huffman.cpp
source/Network/Implementation/Logging.hpp
source/Network/Implementation/Logging.cpp
source/Network/Implementation/MakeUnique.hpp
source/Network/Implementation/NetworkKey.hpp
source/Network/Implementation/NetworkKey.cpp
source/Network/Implementation/NetworkManagerServerGuts.cpp
source/Network/Implementation/NetworkManagerServerGuts.hpp
source/Network/Implementation/NetworkManagerClientGuts.hpp
source/Network/Implementation/NetworkManagerClientGuts.cpp
source/Network/Implementation/NetworkProviderSynchronous.cpp
source/Network/Implementation/NetworkProviderSynchronous.hpp
source/Network/Implementation/NetworkProviderInMemory.cpp
source/Network/Implementation/NetworkProviderInMemory.hpp
source/Network/Implementation/PacketFragmentManager.hpp
source/Network/Implementation/PacketFragmentManager.cpp
source/Network/Implementation/Packet.cpp
source/Network/Implementation/Packets.hpp
source/Network/Implementation/PacketCommand.hpp
source/Network/Implementation/PacketCommandWithKey.hpp
source/Network/Implementation/PacketDelta.cpp
source/Network/Implementation/PacketFragment.cpp
source/Network/Implementation/PacketFragment.hpp
source/Network/Implementation/PacketChallengeResponse.cpp
source/Network/Implementation/PacketChallenge.cpp
source/Network/Implementation/PacketDelta.hpp
source/Network/Implementation/PacketChallenge.hpp
source/Network/Implementation/Packet.hpp
source/Network/Implementation/PacketChallengeResponse.hpp
source/Network/Implementation/Units.hpp
source/Network/Implementation/XorCode.hpp
)

set(NETWORK_TEST
source/Network/Test/TestConnection.cpp
source/Network/Test/TestPackets.cpp
source/Network/Test/TestPacketDelta.cpp
source/Network/Test/TestPacketFragment.cpp
source/Network/Test/TestPacketFragmentManager.cpp
source/Network/Test/TestXorCode.cpp
source/Network/Test/TestBufferSerialisation.cpp
source/Network/Test/TestNetworkProviderSynchronous.cpp
source/Network/Test/TestNetworkProviderInMemory.cpp
source/Network/Test/TestClientServer.cpp
source/Network/Test/TestClientServerN.cpp
source/Network/Test/TestWrappingCounter.cpp
source/Network/Test/TestHuffman.cpp
source/Network/Test/TestBitStreamReadOnly.cpp
source/Network/Test/TestBitStream.cpp
source/Network/Test/MockINetworkProvider.hpp
source/Network/Test/MockIStateManager.hpp
)

# Move source and source network into an "unused" project.
# Change namespace to be the same.
set(COMMON_SOURCE
source/Common/PrecompiledHeaders.cpp
source/Common/PrecompiledHeaders.hpp
source/Common/AutoComplete.hpp
source/Common/ReflectionKey.hpp
source/Common/IReflected.cpp
source/Common/RollingStatistics.cpp
source/Common/AutoComplete.cpp
source/Common/GameSimple.cpp
source/Common/ReflectionManager.cpp
source/Common/GameSimple.hpp
source/Common/ReflectionManager.hpp
source/Common/RollingStatistics.hpp
source/Common/IReflected.hpp
source/Common/BaseClassUniquePointer.hpp
)

set(COMMON_SOURCE_NETWORK
source/Common/Network/DeltaMapItem.hpp
source/Common/Network/DeltaCoder.hpp
source/Common/Network/DeltaCoder-Implementation.hpp
)

###############
# General
###############

# Stupid clang, hard code search paths for ubuntu 12.04 for travis-ci
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    include_directories(/usr/include/x86_64-linux-gnu/c++/4.8/)
    include_directories(/usr/include/i386-linux-gnu/c++/4.8/)
endif()

enable_testing()

###############
# Testing
###############
# yikes! enable google test. Since this doesn't have a module, do it manually.
# http://stackoverflow.com/questions/9689183/cmake-googletest/9695234#9695234
# yikes! that wasted a day of my life. Doesn't work as it can't find a make target
# Maybe it only works in windows? Either way, include gmock with the source.

add_subdirectory(third-party/gmock-1.6.0)
include_directories(SYSTEM ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

###############
# Network
###############
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/Network/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/Network/)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/Network/Implementation)

add_library(
game-in-a-box-network
${NETWORK_HEADERS}
${NETWORK}
)

set_target_properties(game-in-a-box-network PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS} ${IGNOREWARNINGS_FLAGS}")

# Testing
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/Network/Test)

ADD_EXECUTABLE(
game-in-a-box-network-tests
${NETWORK_TEST}
)

# Ignore google test and goole mock warnings
set_target_properties(game-in-a-box-network-tests PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS} ${GMOCK_FLAGS} ${IGNOREWARNINGS_FLAGS}")

target_link_libraries(
  game-in-a-box-network-tests
  game-in-a-box-network
  ${CMAKE_THREAD_LIBS_INIT}
  gmock
  gmock_main
  ${Boost_LIBRARIES})

add_test(
  NAME game-in-a-box-network-tests
  COMMAND game-in-a-box-network-tests
)

###############
# Server 
###############    
#add_executable(
#game-in-a-box-server
#source/Server/GameInABoxServer.cpp
#source/Server/main.cpp)

#set_target_properties(game-in-a-box-server PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS} ${IGNOREWARNINGS_FLAGS}")
#target_link_libraries(game-in-a-box-server game-in-a-box-common ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

# RAM: TODO: Don't do this for cross compilation builds.
#add_custom_command(TARGET game-in-a-box-server POST_BUILD COMMAND game-in-a-box-common-tests)
    
###############
# Client 
###############
#source/Network/Test/TestDeltaCoder.cpp
