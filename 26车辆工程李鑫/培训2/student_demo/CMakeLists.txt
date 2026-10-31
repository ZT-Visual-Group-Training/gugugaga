cmake_minimum_required(VERSION 3.10)
project(StudentDemo)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/includes)

add_library(student STATIC tools/student.cpp)

add_executable(student_test src/main.cpp)

target_link_libraries(student_test student)
