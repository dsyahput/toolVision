cmake_minimum_required(VERSION 3.0.0)
project(toolVision)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(LINK_SHARED_ZED "Link with the ZED SDK shared executable" ON)

if (NOT LINK_SHARED_ZED AND MSVC)
    message(FATAL_ERROR "LINK_SHARED_ZED OFF : ZED SDK static libraries not available on Windows")
endif()

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 OLD)
	cmake_policy(SET CMP0015 OLD)
endif(COMMAND cmake_policy)

SET(EXECUTABLE_OUTPUT_PATH ".")

find_package(Qt5 REQUIRED COMPONENTS Core)

find_package(Boost REQUIRED COMPONENTS
             thread chrono filesystem)

find_package(OpenCV REQUIRED)
find_package(ZED 3 REQUIRED)
find_package(CUDA REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Boost 1.56 REQUIRED COMPONENTS
             thread chrono)
include_directories("include/")
include_directories("include/barelang_loc")

IF(NOT WIN32)
    add_definitions(-Wno-format-extra-args)
    SET(SPECIAL_OS_LIBS "pthread" "X11")
ENDIF()

include_directories(${ZED_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})

link_directories(${ZED_LIBRARY_DIR})
link_directories(${OpenCV_LIBRARY_DIRS})
link_directories(${CUDA_LIBRARY_DIRS})

# Define the build type
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")

add_library(vision
src/lib/barelangTrackbar.cpp
src/lib/cameraVision.cpp
src/lib/PID.cpp         
)

target_link_libraries(vision Qt5::Core Boost::thread Boost::chrono)

# Add executable
add_executable(capture_omni 
               src/capture_omni.cpp)

add_executable(capture_zed 
               src/capture_zed.cpp)

add_executable(omni_seg_video 
             src/omni_seg_video.cpp)

add_executable(omni_seg_camera 
             src/omni_seg_camera.cpp)

add_executable(testing 
             src/testing.cpp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Target compile features
target_compile_features(capture_omni PRIVATE cxx_std_17)
target_compile_features(capture_zed  PRIVATE cxx_std_17)

if (LINK_SHARED_ZED)
    SET(ZED_LIBS ${ZED_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_LIBRARIES})
else()
    SET(ZED_LIBS ${ZED_STATIC_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_LIBRARIES})
endif()

# Link libraries
target_link_libraries(capture_omni ${OpenCV_LIBS} vision
                     yaml-cpp
                     udev)

target_link_libraries(capture_zed 
                     ${ZED_LIBS} ${OpenCV_LIBRARIES})

target_link_libraries(omni_seg_video ${OpenCV_LIBS} vision
                     yaml-cpp
                     udev
                     )

target_link_libraries(omni_seg_camera ${OpenCV_LIBS} vision
                     yaml-cpp
                     udev
                     )

target_link_libraries(testing ${OpenCV_LIBS} vision
                     yaml-cpp
                     udev
                     )