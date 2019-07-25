cmake_minimum_required(VERSION 3.8)
project(faces)

option(USE_DLIB "Compile with dlib?" ON)
set(DLIB_PATH $ENV{HOME}/dlib)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)

set(SOURCE_FILES main.cpp Detector.cpp Detector.h utils.hpp Face.cpp Face.h Callbacks.hpp Recognizer.cpp Recognizer.h Faces.cpp Faces.h)
add_executable(faces ${SOURCE_FILES})

target_link_libraries(faces ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(faces ${OpenCV_LIBS})

if (${USE_DLIB})
    message(STATUS "Using Dlib")
    add_subdirectory(${DLIB_PATH} dlib_build)
    target_link_libraries(faces dlib)
    add_compile_definitions(USE_DLIB)
else ()
    message(STATUS "Not using Dlib")
endif ()
