cmake_minimum_required(VERSION 3.10)

project(hsv_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(OpenCV REQUIRED)

add_executable(hsv_demo main.cpp)

target_include_directories(hsv_demo PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(hsv_demo PRIVATE ${OpenCV_LIBS})

# 把测试图片拷到构建目录，这样在 build/ 里运行也能读到 armor.jpg
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/armor.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

