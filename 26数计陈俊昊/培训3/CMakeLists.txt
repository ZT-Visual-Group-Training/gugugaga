cmake_minimum_required(VERSION 3.10)
project(Armor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 找 OpenCV（这一步失败就说明环境没配好，别往下看）
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")

# 自己写的头文件目录
include_directories(${PROJECT_SOURCE_DIR}/includes)
# OpenCV 的头文件目录
include_directories(${OpenCV_INCLUDE_DIRS})

# add_executable 只列 .cpp，头文件不列
add_executable(app
    src/main.cpp
    src/armor.cpp
)

# 链接 OpenCV 的库
target_link_libraries(app ${OpenCV_LIBS})
