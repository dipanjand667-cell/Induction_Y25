#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

echo "=> Building the multi-stage Docker image..."
docker build -t drone_fleet_image:latest .

echo "=> Launching the ARIITK Fleet Simulation..."
# -it: Interactive terminal
# --rm: Clean up container after exit
# --net=host: Share host network stack (crucial for ROS 2 DDS discovery)
# -e ROS_DOMAIN_ID: Isolate our DDS traffic
# -v: Mount the local src directory to the container's src directory

docker run -it --rm \
    --net=host \
    -e ROS_DOMAIN_ID=42 \
    -v "$(pwd)/src:/ros2_ws/src" \
    drone_fleet_image:latest \
    ros2 launch drone_fleet fleet.launch.py
