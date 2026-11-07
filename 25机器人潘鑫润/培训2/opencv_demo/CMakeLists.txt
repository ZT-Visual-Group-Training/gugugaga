cmake_minimum_required(VERSION 3.11)
project(opencv_demo)

# 1. 查找 OpenCV 包
find_package(OpenCV REQUIRED)

# 2. 包含头文件目录
include_directories(${OpenCV_INCLUDE_DIRS})

# 3. 生成可执行文件
add_executable(main main.cpp)

# 4. 链接 OpenCV 库
target_link_libraries(main ${OpenCV_LIBS})


