cmake_minimum_required(VERSION 3.8)
project(faces)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)

set(SOURCE_FILES main.cpp Recognizer.cpp Recognizer.h utils.hpp Face.cpp Face.h Callbacks.hpp)
add_executable(faces ${SOURCE_FILES})

target_link_libraries(faces ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(faces ${OpenCV_LIBS})