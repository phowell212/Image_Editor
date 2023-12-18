cmake_minimum_required(VERSION 3.28)
project(Image_Editor_Test)

set(CMAKE_CXX_STANDARD 23)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories for OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executable
add_executable(Image_Editor_Test
        main.cpp)

# Link OpenCV libraries
target_link_libraries(Image_Editor_Test ${OpenCV_LIBS})
