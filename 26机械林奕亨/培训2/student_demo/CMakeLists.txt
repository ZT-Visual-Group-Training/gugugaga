cmake_minimum_required(VERSION 3.10)
project(student_demo)

add_library(student STATIC tools/student.cpp)
target_include_directories(student PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)

add_executable(student_demo src/main.cpp)
target_link_libraries(student_demo PRIVATE student)
