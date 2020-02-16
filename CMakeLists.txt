cmake_minimum_required(VERSION 3.8)
project(faces)

set(DLIB_PATH $ENV{HOME}/dlib)

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4 -march=native")
# set(USE_SSE4_INSTRUCTIONS ON)
# set(USE_AVX_INSTRUCTIONS ON)

# For profiling
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

find_package(Threads REQUIRED)
find_package(OpenCV 4 REQUIRED)
find_package(nlohmann_json REQUIRED)

set(SOURCE_FILES Detection/Detection.cpp Detection/Detection.h utils/utils.hpp Face/Face.cpp Face/Face.h Callbacks.hpp Recognition/recognition.cpp Recognition/recognition.h Faces.cpp Faces.h Recognition/MyClassifier.hpp Recognition/RecognitionNet.hpp FaceChecker.cpp FaceChecker.h utils/Config.cpp utils/Config.h utils/Serialization.hpp)
add_library(faces STATIC ${SOURCE_FILES})

target_link_libraries(faces ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(faces ${OpenCV_LIBS})
target_link_libraries(faces nlohmann_json::nlohmann_json)

add_subdirectory(${DLIB_PATH} dlib_build)
target_link_libraries(faces dlib)

add_executable(run main.cpp)
target_link_libraries(run faces)