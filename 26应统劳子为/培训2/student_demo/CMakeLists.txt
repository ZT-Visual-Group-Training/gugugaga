cmake_minimum_required(VERSION 3.10)
project(student_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(student STATIC
    tools/students.cpp
)

target_include_directories(student PUBLIC ${PROJECT_SOURCE_DIR}/includes)

add_executable(app scr/main.cpp)
target_link_libraries(app PRIVATE student)
