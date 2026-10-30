cmake_minimum_required(VERSION 3.15)

project(opencv_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 用 find_package 查找 OpenCV。
# 如果 CMake 找不到，可以在配置时手动指定（OpenCV 4.x 目录叫 opencv4，5.x 叫 opencv5）：
#   cmake -S . -B build -DOpenCV_DIR=<OpenCV 安装目录>/lib/cmake/opencv5
find_package(OpenCV REQUIRED)

message(STATUS "找到 OpenCV 版本: ${OpenCV_VERSION}")
message(STATUS "OpenCV 头文件目录: ${OpenCV_INCLUDE_DIRS}")

add_executable(opencv_demo main.cpp)

target_include_directories(opencv_demo PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(opencv_demo PRIVATE ${OpenCV_LIBS})
