cmake_minimum_required(VERSION 3.8)
project(faces)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

set(SOURCE_FILES main.cpp FaceRecognizer.cpp FaceRecognizer.h)
add_executable(faces ${SOURCE_FILES})

target_link_libraries(faces ${OpenCV_LIBS})
