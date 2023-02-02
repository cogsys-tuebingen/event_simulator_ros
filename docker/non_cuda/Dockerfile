FROM osrf/ros:noetic-desktop

# Minimal setup
RUN apt-get update && \
    apt-get install -y locales lsb-release
ARG DEBIAN_FRONTEND=noninteractive
RUN dpkg-reconfigure locales

# Get dependencies
RUN apt-get update && apt-get install -y git gdb cmake-curses-gui

# Install metavision
RUN apt-get install -y libcanberra-gtk-module mesa-utils ffmpeg
RUN apt-get install -y cmake libboost-program-options-dev libeigen3-dev
RUN apt-get install -y python3-pip python3-tk
RUN apt-get install -y python3.8-dev
RUN python3 -m pip install pip --upgrade
RUN python3 -m pip install "opencv-python>=4.5.5.64" "sk-video==1.1.10" "fire==0.4.0" "numpy<=1.21" pandas scipy h5py
RUN python3 -m pip install jupyter jupyterlab matplotlib "ipywidgets==7.6.5"

ADD ../metavision.list /etc/apt/sources.list.d/metavision.list
RUN apt-get update && apt-get install -y metavision-sdk

# Install profiling tools
RUN apt-get update && apt-get install -y linux-tools-generic hotspot
RUN rm /usr/bin/perf
RUN ln -s /usr/lib/linux-tools/5.4.0-80-generic/perf /usr/bin/perf

# Add utility scripts
RUN apt -y install jq
ADD ../merge-compile-commands /usr/local/bin/merge-compile-commands
RUN chmod +x /usr/local/bin/merge-compile-commands

ADD ../catkin-update-compile-commands /usr/local/bin/catkin-update-compile-commands
RUN chmod +x /usr/local/bin/catkin-update-compile-commands
