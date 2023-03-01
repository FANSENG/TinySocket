cmake_minimum_required(VERSION 3.10)

project(main VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED True)

# 生成动态链接库
# add_library(main SHARED main.cpp printhello.cpp utils.cpp)

# 添加子目录
# add_subdirectory(math)

# 添加链接库
# target_link_libraries(Demo MathFunctions)

add_executable( client
    src/client.cc
)

add_executable( server
    src/server.cc
)