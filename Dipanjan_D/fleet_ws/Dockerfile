# ==========================================
# STAGE 1: Builder
# ==========================================
FROM osrf/ros:humble-desktop AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    python3-colcon-common-extensions \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /ros2_ws

# Copy the source code
COPY src/ ./src/

# Build the workspace
RUN /bin/bash -c "source /opt/ros/jazzy/setup.bash && colcon build"

# ==========================================
# STAGE 2: Runtime
# ==========================================
FROM ros:jazzy-ros-base AS runtime

WORKDIR /ros2_ws

# Copy ONLY the compiled install directory from the builder
COPY --from=builder /ros2_ws/install /ros2_ws/install

# Create a dynamic entrypoint to source ROS 2 and the workspace automatically
RUN echo '#!/bin/bash\nset -e\nsource /opt/ros/jazzy/setup.bash\nsource /ros2_ws/install/setup.bash\nexec "$@"' > /entrypoint.sh \
    && chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]

# Default command if none is provided
CMD ["/bin/bash"]
