cmake_minimum_required(VERSION 3.10)
project(geometry)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Boost 库 (可以指定需要的组件，例如system、filesystem等)
find_package(Boost REQUIRED)

# 添加 Boost 头文件路径
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
)

# 添加可执行文件，假设源文件为 main.cpp
add_executable(geometry source/main.cpp)

# 链接 Boost 库到可执行文件
target_link_libraries(geometry ${Boost_LIBRARIES})