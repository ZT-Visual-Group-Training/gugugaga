cmake_minimum_required(VERSION 3.15)

project(student_demo LANGUAGES CXX)

# 使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------
# 1. 先把 Student 的实现编译成静态库（Windows 下是 student.lib，
#    MinGW / Linux 下是 libstudent.a）
# ---------------------------------------------------------------
add_library(student STATIC
    tools/student.cpp
)

# 头文件目录对外公开，链接静态库的目标会自动继承
target_include_directories(student PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

# ---------------------------------------------------------------
# 2. 可执行文件链接上面这个静态库
# ---------------------------------------------------------------
add_executable(student_demo
    src/main.cpp
)

target_link_libraries(student_demo PRIVATE student)
