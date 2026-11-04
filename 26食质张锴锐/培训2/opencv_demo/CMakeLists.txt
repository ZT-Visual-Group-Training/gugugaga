cmake_minimum_required(VERSION 3.10)
project(opencv_demo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找系统已安装的 OpenCV（作业要求使用 find_package）
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV 版本: ${OpenCV_VERSION}")

# 主程序
add_executable(app main.cpp)

# 链接 OpenCV 库
target_link_libraries(app PRIVATE ${OpenCV_LIBS})
target_include_directories(app PRIVATE ${OpenCV_INCLUDE_DIRS})
