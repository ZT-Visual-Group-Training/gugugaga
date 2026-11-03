cmake_minimum_required(VERSION 3.16)
project(hsv_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 不绑主版本：环境是 OpenCV 4.x 或 5.x 都能找到
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV: ${OpenCV_VERSION}")

add_executable(hsv_demo main.cpp)

target_include_directories(hsv_demo PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(hsv_demo PRIVATE ${OpenCV_LIBS})

# 把测试图片拷到构建目录，这样在 build/ 里直接运行也能找到 armor.jpg
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/armor.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
