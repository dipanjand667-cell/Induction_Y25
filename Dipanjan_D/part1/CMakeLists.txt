cmake_minimum_required(VERSION 3.10)
project(part1_assignment)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The autograder explicitly checks for these compiler flags!
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Tell CMake to look in the 'include' folder for your .hpp files
include_directories(include)

# Compile all the lowercase .cpp files into a single executable
add_executable(drone_sim 
    src/main.cpp 
    src/vehicle.cpp 
    src/drone.cpp 
    src/mission_drone.cpp 
    src/autonomous_drone.cpp
)
