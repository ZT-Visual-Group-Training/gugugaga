cmake_minimum_required(VERSION 3.10)
project(Student)

add_library(student_demo STATIC
            tools/Student.cpp
)
include_directories(./includes)

add_executable(main src/main.cpp)
target_link_libraries(main student_demo)