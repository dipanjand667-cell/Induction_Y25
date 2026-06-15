cmake_minimum_required(VERSION 3.8)
project(drone_fleet)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)

include_directories(include include/drone_fleet)

# Compile OOP source files into a library
add_library(oop_core 
  src/oop/Vehicle.cpp 
  src/oop/Drone.cpp 
  src/oop/MissionDrone.cpp 
  src/oop/AutonomousDrone.cpp
)

# Drone Node
add_executable(drone_node src/drone_node.cpp)
ament_target_dependencies(drone_node rclcpp std_msgs)
target_link_libraries(drone_node oop_core)

# Fleet Manager Node
add_executable(fleet_manager src/fleet_manager.cpp)
ament_target_dependencies(fleet_manager rclcpp std_msgs std_srvs)

# Health Monitor Node
add_executable(health_monitor src/health_monitor.cpp)
ament_target_dependencies(health_monitor rclcpp std_msgs)

install(TARGETS drone_node fleet_manager health_monitor
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
