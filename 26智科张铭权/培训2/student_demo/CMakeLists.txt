cmake_minimum_required(VERSION 3.10)
project(student_demo)

set(CMAKE_CXX_STANDARD 11)

# 把头文件目录加入搜索路径
include_directories(includes)

# 把student.cpp编译成静态库
add_library(student STATIC tools/student.cpp)

# 编译主程序，链接静态库
add_executable(main src/main.cpp)
target_link_libraries(main student)
