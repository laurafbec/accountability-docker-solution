#!/bin/bash
set -e

# Setup ROS environment
source "/opt/ros/$ROS_DISTRO/setup.bash"

# Setup workspace
if [ -n "$WORKSPACE_NAME" ]; then
   if [ ! -e "/root/$WORKSPACE_NAME/devel/setup.bash" ]; then
      previousDirectory=$(pwd)
      cd /root/$WORKSPACE_NAME
      catkin_make
      cd $previousDirectory
    fi
    source "/root/$WORKSPACE_NAME/devel/setup.bash"
fi


exec "$@"
