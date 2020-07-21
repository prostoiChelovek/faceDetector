cmake_minimum_required(VERSION 3.13)
project(faces)

set(CMAKE_CXX_STANDARD 17)

find_package(spdlog REQUIRED)
find_package(OpenCV 4 REQUIRED)

add_library(faces "")

target_link_libraries(faces ${OpenCV_LIBS})
target_link_libraries(faces spdlog::spdlog)

include_directories(src)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(doc)
