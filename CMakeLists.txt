
# set Cmake minimum version to use
# Current version : cmake version 3.5.1
cmake_minimum_required(VERSION 2.8.9)

# set the compiler
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")
set (CMAKE_CXX_STANDARD 11)

# name your project
project(MULTISTORKE_RECOGNITION)
set(CMAKE_BUILD_TYPE Release)

# Find and load the settings from the packages
find_package( OpenCV REQUIRED )

# set the extra flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -cflags -libs")

# set the directory that the executable file is going to build
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#set the shared libraries name
set(PROJECT_LINK_LIBS ${OpenCV_LIBS} )

# include the header files (.h)
include_directories(${PROJECT_SOURCE_DIR}/Header)

# add the sources using the set command as follow
set(SOURCES
Source/MultiStrokeRec.cpp
Source/DirAccess.cpp
Source/Classification.cpp
Source/ImageProcessing.cpp
Source/GeometricRecognizer.cpp
)

# add_executable(name_of_executable.o list_of_cpp_files)
add_executable(MultistrokeRec ${SOURCES})

# link the executable with the shared libraries
target_link_libraries(MultistrokeRec ${PROJECT_LINK_LIBS} )
