cmake_minimum_required(VERSION 3.10)
project(student_demo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1) student 静态库（源码在 includes/tools/Student.cpp，头文件目录 includes/）
add_library(student STATIC
    includes/tools/Student.cpp
)
target_include_directories(student PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

# 2) 主程序，链接 student 静态库
add_executable(app
    includes/tools/src/main.cpp
)
target_link_libraries(app PRIVATE student)
