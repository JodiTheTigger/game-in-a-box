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
check_cxx_compiler_flag("-fstrict-aliasing" CAN_DISABLE_ALIASING)
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

if (CAN_DISABLE_ALIASING)
    # Tell the compiler I'm not doing anything fancy like aliasing my pointers.
    # But watch out, this can cause all sorts of weird, unwarnable bugs if you're
    # Not careful.
    # http://dbp-consulting.com/tutorials/StrictAliasing.html
    set(PARANOID_FLAGS "${PARANOID_FLAGS} -fstrict-aliasing")
endif()


###############
# Third Party Libraries
###############

# Clang needs 1.48 otherwise it doesn't build due to calling a deleted constructor.
FIND_PACKAGE( Boost 1.48 COMPONENTS system thread REQUIRED )
INCLUDE_DIRECTORIES( SYSTEM ${Boost_INCLUDE_DIR} )

# Uses threads in Linuxland
FIND_PACKAGE ( Threads )

###############
# Source Code
###############
set(COMMON_HEADERS
include/Common/No.hpp
include/Common/BitStream.hpp
include/Common/BitStreamReadOnly.hpp
include/Common/BufferSerialisation.hpp
include/Common/MakeUnique.hpp
include/Common/Logging.hpp
include/Common/UnitOperators.hpp
include/Common/MathConstants.hpp
)

set(NETWORK_HEADERS
include/Network/ClientHandle.hpp
include/Network/Delta.hpp
include/Network/INetworkManager.hpp
include/Network/INetworkProvider.hpp
include/Network/IStateManager.hpp
include/Network/NetworkManagerClient.hpp
include/Network/NetworkManagerServer.hpp
include/Network/NetworkPacket.hpp
include/Network/Sequence.hpp
include/Network/WrappingCounter.hpp
include/Network/MathBitTwiddling.hpp
include/Network/Units.hpp
include/Network/DeltaCreator.hpp
include/Network/DeltaParser.hpp
include/Network/DeltaCoder.hpp
include/Network/DeltaCoder.tpp
include/Network/DeltaCoderTools.hpp
include/Network/DeltaMapItem.hpp
include/Network/DeltaTypes.hpp
include/Network/ClientTracker.hpp
)

set(COMMON
source/Common/BitStream.cpp
source/Common/BitStreamReadOnly.cpp
source/Common/Logging.cpp
)

set(NETWORK
source/Network/DeltaMapItem.cpp
source/Network/DeltaCoderTools.cpp
source/Network/INetworkManager.cpp
source/Network/INetworkProvider.cpp
source/Network/IStateManager.cpp
source/Network/NetworkManagerClient.cpp
source/Network/NetworkManagerServer.cpp
source/Network/ClientTracker.cpp
source/Network/Implementation/Connection.cpp
source/Network/Implementation/Connection.hpp
source/Network/Implementation/Hash.hpp
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

set(COMMON_TEST
test/Common/TestBitStreamReadOnly.cpp
test/Common/TestBitStream.cpp
test/Common/TestBufferSerialisation.cpp
)

set(NETWORK_TEST
test/Network/TestConnection.cpp
test/Network/TestDeltaCoder.cpp
test/Network/TestPackets.cpp
test/Network/TestPacketDelta.cpp
test/Network/TestPacketFragment.cpp
test/Network/TestPacketFragmentManager.cpp
test/Network/TestXorCode.cpp
test/Network/TestNetworkProviderSynchronous.cpp
test/Network/TestNetworkProviderInMemory.cpp
test/Network/TestClientServer.cpp
test/Network/TestClientServerN.cpp
test/Network/TestWrappingCounter.cpp
test/Network/MockINetworkProvider.hpp
test/Network/MockIStateManager.hpp
)

set(STATE
source/State/Implementation/DeltaStatePlayerClient.cpp
source/State/Implementation/DeltaStatePlayerClient.hpp
source/State/Implementation/DeltaStateGameSnapshot.cpp
source/State/Implementation/DeltaStateGameSnapshot.hpp
source/State/Implementation/Types.hpp
source/State/Implementation/Game.hpp
source/State/Implementation/Game.cpp
source/State/Implementation/PhysicsCube.hpp
source/State/Implementation/PhysicsPlayers.cpp
source/State/Implementation/PhysicsPlayers.hpp
source/State/Implementation/GameNetworkClient.hpp
source/State/Implementation/GameNetworkClient.cpp
)

set(STATE_TEST
test/State/TestDeltaStatePlayerClient.cpp
test/State/TestDeltaStateGameSnapshot.cpp
test/State/TestGame.cpp
)

set(UNUSED
source/Unused/AutoComplete.cpp
source/Unused/AutoComplete.hpp
source/Unused/IReflected.cpp
source/Unused/IReflected.hpp
source/Unused/GameSimple.cpp
source/Unused/GameSimple.hpp
source/Unused/ReflectionManager.cpp
source/Unused/ReflectionManager.hpp
source/Unused/RollingStatistics.hpp
source/Unused/RollingStatistics.cpp
source/Unused/ReflectionKey.hpp
source/Unused/BaseClassUniquePointer.hpp
source/Unused/Huffman.hpp
source/Unused/Huffman.cpp
)

set(UNUSED_TEST
test/Unused/TestAutoComplete.cpp
test/Unused/TestIReflected.cpp
test/Unused/TestReflectionManager.cpp
test/Unused/TestRollingStatistics.cpp
test/Unused/TestHuffman.cpp
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
# Third party
###############
# for now include third party via source, as opposed to getting CMake to download
# it at compile time.

add_subdirectory(third-party/gmock-1.6.0)
include_directories(SYSTEM ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

add_subdirectory(third-party/bullet-2.x)
include_directories(SYSTEM ${BULLET_PHYSICS_SOURCE_DIR}/src)

###############
# Common
###############

# Everyone has access to include.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/Common/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/Common/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test/Common/)

add_library(
game-in-a-box-common
${COMMON_HEADERS}
${COMMON}
)

set_target_properties(game-in-a-box-common PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS} ${IGNOREWARNINGS_FLAGS}")

ADD_EXECUTABLE(
game-in-a-box-common-tests
${COMMON_TEST}
)

# Ignore google test and goole mock warnings
set_target_properties(game-in-a-box-common-tests PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS} ${GMOCK_FLAGS} ${IGNOREWARNINGS_FLAGS}")

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

# run tests after every build please.
add_custom_command(TARGET game-in-a-box-common-tests POST_BUILD COMMAND game-in-a-box-common-tests)

###############
# Network
###############
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/Network/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/Network/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test/Network/)

add_library(
game-in-a-box-network
${NETWORK_HEADERS}
${NETWORK}
)

set_target_properties(game-in-a-box-network PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS} ${IGNOREWARNINGS_FLAGS}")

ADD_EXECUTABLE(
game-in-a-box-network-tests
${NETWORK_TEST}
)

# Ignore google test and goole mock warnings
set_target_properties(game-in-a-box-network-tests PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS} ${GMOCK_FLAGS} ${IGNOREWARNINGS_FLAGS}")

target_link_libraries(
  game-in-a-box-network-tests
  game-in-a-box-network
  game-in-a-box-common
  ${CMAKE_THREAD_LIBS_INIT}
  gmock
  gmock_main
  ${Boost_LIBRARIES})

add_test(
  NAME game-in-a-box-network-tests
  COMMAND game-in-a-box-network-tests
)

# run tests after every build please.
add_custom_command(TARGET game-in-a-box-network-tests POST_BUILD COMMAND game-in-a-box-network-tests)

###############
# State
###############
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/State/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/State/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test/State/)

add_library(
game-in-a-box-state
#${STATE_HEADERS}
${STATE}
)

set_target_properties(game-in-a-box-state PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS} ${IGNOREWARNINGS_FLAGS}")

ADD_EXECUTABLE(
game-in-a-box-state-tests
${STATE_TEST}
)

# Ignore google test and goole mock warnings
set_target_properties(game-in-a-box-state-tests PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS} ${GMOCK_FLAGS} ${IGNOREWARNINGS_FLAGS}")

target_link_libraries(
  game-in-a-box-state-tests
  game-in-a-box-state
  game-in-a-box-network
  game-in-a-box-common
  ${CMAKE_THREAD_LIBS_INIT}
  gmock
  gmock_main
  BulletDynamics
  BulletCollision
  LinearMath
  ${Boost_LIBRARIES})

add_test(
  NAME game-in-a-box-state-tests
  COMMAND game-in-a-box-state-tests
)

# run tests after every build please.
add_custom_command(TARGET game-in-a-box-state-tests POST_BUILD COMMAND game-in-a-box-state-tests)

###############
# Unused
###############
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/Unused/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test/Unused/)

add_library(
game-in-a-box-unused
${UNUSED}
)

set_target_properties(game-in-a-box-unused PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS} ${IGNOREWARNINGS_FLAGS}")

# Testing
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/Unused/Test)

ADD_EXECUTABLE(
game-in-a-box-unused-tests
${UNUSED_TEST}
)

# Ignore google test and goole mock warnings
set_target_properties(game-in-a-box-unused-tests PROPERTIES COMPILE_FLAGS "${PARANOID_FLAGS} ${GMOCK_FLAGS} ${IGNOREWARNINGS_FLAGS}")

# Somce test use bitstream, which makes them dependendent on Network
target_link_libraries(
  game-in-a-box-unused-tests
  game-in-a-box-unused
  game-in-a-box-network
  game-in-a-box-common
  ${CMAKE_THREAD_LIBS_INIT}
  gmock
  gmock_main
  ${Boost_LIBRARIES})


add_test(
  NAME game-in-a-box-unused-tests
  COMMAND game-in-a-box-unused-tests
)

# run tests after every build please.
add_custom_command(TARGET game-in-a-box-unused-tests POST_BUILD COMMAND game-in-a-box-unused-tests)

###############
# Server 
###############
# Commented out until I need it, will reuse when I'm in the state to
# Do a full void main() executable for a client, server, and listen server client exe.
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
# ???
