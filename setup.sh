#!/bin/bash
set -e

# Kernel headers installation are required by Sysdig
echo "Upting list of available packages and installing system dependencies ..."

# Debian-like distributions
if [ -f /etc/lsb-release ]; then
  sudo apt-get -y update
  sudo apt-get -y install linux-headers-$(uname -r)
fi

# RHEL-like distributions
if [ -f /etc/redhat-release ]; then
   sudo yum -y update
   sudo yum -y install kernel-devel-$(uname -r)
fi

# Docker-compose install
sudo curl -L "https://github.com/docker/compose/releases/download/1.26.0/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose

#.bashrc update
# UID and GID variables are required by MongoDB (mongodb/data/db owner and group)
if ! grep -q UID "$HOME/.bashrc"; then
  echo -e "export UID=\$(id -u)\nexport GID=\$(id -g)" >> "$HOME/.bashrc"
  source "$HOME/.bashrc"
fi
