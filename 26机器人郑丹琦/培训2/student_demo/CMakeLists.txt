cmake_minimum_required(VERSION 3.10)
project(student_demo CXX)
set(CMAKE_CXX_STANDARD 17)

add_library(student STATIC tools/student.cpp)
target_include_directories(student PUBLIC includes)

add_executable(student_demo src/main.cpp)
target_link_libraries(student_demo PRIVATE student)
