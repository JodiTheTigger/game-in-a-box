project(game-in-a-box-server CXX)

add_executable(game-in-a-box-server common/BitStream.cpp server/main.cpp)

set(CMAKE_CXX_FLAGS "-std=c++0x")

