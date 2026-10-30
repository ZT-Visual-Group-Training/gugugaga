cmake_minimum_required(VERSION 3.10)
project(StudentDemo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/includes)

add_library(student STATIC
    tools/student.cpp
)
link_libraries(student)
add_executable(student_app
    src/main.cpp
)
