cmake_minimum_required(VERSION 3.8)
project(faces)

set(DLIB_PATH $ENV{HOME}/dlib)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4 -march=native")
#set(USE_SSE4_INSTRUCTIONS ON)
#set(USE_AVX_INSTRUCTIONS ON)

find_package(Threads REQUIRED)
find_package(OpenCV 4 REQUIRED)

set(SOURCE_FILES Detector.cpp Detector.h utils.hpp Face.cpp Face.h Callbacks.hpp Recognizer.cpp Recognizer.h Faces.cpp Faces.h MyClassifier.hpp RecognitionNet.hpp Recognizer_LBPH.cpp Recognizer_LBPH.h Recognizer_Descriptors.cpp Recognizer_Descriptors.h FaceChecker.cpp FaceChecker.h)
add_library(faces STATIC ${SOURCE_FILES})

target_link_libraries(faces ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(faces ${OpenCV_LIBS})

add_subdirectory(${DLIB_PATH} dlib_build)
target_link_libraries(faces dlib)

add_executable(run main.cpp)
target_link_libraries(run faces)