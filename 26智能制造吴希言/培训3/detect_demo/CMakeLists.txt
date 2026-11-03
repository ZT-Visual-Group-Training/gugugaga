cmake_minimum_required(VERSION 3.16)
project(detect_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 不绑主版本：环境是 OpenCV 4.x 或 5.x 都能找到
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV: ${OpenCV_VERSION}")

add_executable(detect_demo main.cpp)

target_include_directories(detect_demo PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(detect_demo PRIVATE ${OpenCV_LIBS})

# 把测试视频拷到构建目录，这样在 build/ 里直接运行也能找到 test_clip.mp4
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_clip.mp4 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
