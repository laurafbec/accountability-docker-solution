#!/bin/bash
set -e

# Pull ROS Noetic image
docker build . -t ros:mynoetic

#Check workspace/src exists
if [ ! -d workspace/src ]; then
	mkdir -p workspace/src
fi
