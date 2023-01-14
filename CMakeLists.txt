cmake_minimum_required(VERSION 2.8)
project(project_4)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${OpenCV_INCLUDE_DIRS})
# add_executable(project_4 main.cpp)
# add_executable(project_4 Task4_6.cpp)
# add_executable(project_4 Task_7.cpp)
add_executable(project_4 Extns.cpp)

target_link_libraries(project_4 ${OpenCV_LIBS})
