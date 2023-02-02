FROM nvidia/cuda:11.8.0-devel-ubuntu20.04

# Minimal setup
RUN apt-get update && \
    apt-get install -y locales lsb-release
ARG DEBIAN_FRONTEND=noninteractive
RUN dpkg-reconfigure locales

# Get dependencies
RUN apt-get update && apt-get install -y git wget gdb cmake-curses-gui gcc build-essential

# OpenCV
RUN apt-get install -y build-essential cmake pkg-config unzip yasm git checkinstall
RUN apt-get install -y libjpeg-dev libpng-dev libtiff-dev
RUN apt-get install -y libavcodec-dev libavformat-dev libswscale-dev libavresample-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libxvidcore-dev x264 libx264-dev libfaac-dev libmp3lame-dev libtheora-dev libfaac-dev libmp3lame-dev libvorbis-dev
RUN apt-get install -y libdc1394-22 libdc1394-22-dev libxine2-dev libv4l-dev v4l-utils
RUN cd /usr/include/linux && ln -s -f ../libv4l1-videodev.h videodev.h
RUN apt-get install -y libgtk-3-dev
RUN apt-get install -y libtbb-dev
RUN apt-get install -y libatlas-base-dev gfortran
RUN apt-get install -y libprotobuf-dev protobuf-compiler libgoogle-glog-dev libgflags-dev libgphoto2-dev libeigen3-dev libhdf5-dev doxygen

RUN cd ~ && \
    wget -O opencv.zip https://github.com/opencv/opencv/archive/4.6.0.zip && \
    wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.6.0.zip && \
    unzip opencv.zip && \
    unzip opencv_contrib.zip && \
    cd opencv-4.6.0 && \
    mkdir build && cd build && \
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
          -D CMAKE_C_COMPILER=/usr/bin/gcc-9 \
          -D CMAKE_INSTALL_PREFIX=/usr/local \
          -D INSTALL_PYTHON_EXAMPLES=ON \
          -D INSTALL_C_EXAMPLES=OFF \
          -D WITH_TBB=ON \
          -D BUILD_opencv_cudacodec=ON \
          -D ENABLE_FAST_MATH=1 \
          -D CUDA_FAST_MATH=1 \
          -D WITH_CUDA=ON \
          -D WITH_CUBLAS=1 \
          -D WITH_V4L=ON \
          -D WITH_QT=OFF \
          -D WITH_OPENGL=ON \
          -D WITH_GSTREAMER=ON \
          -D OPENCV_GENERATE_PKGCONFIG=ON \
          -D OPENCV_PC_FILE_NAME=opencv.pc \
          -D OPENCV_ENABLE_NONFREE=ON \
          -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib-4.6.0/modules \
          -D PYTHON_EXECUTABLE=~/.virtualenvs/cv/bin/python \
          -D BUILD_TESTS=ON \
          -D BUILD_PERF_TESTS=ON \
          -D BUILD_EXAMPLES=OFF \
          -D WITH_CUDNN=OFF \
          -D OPENCV_DNN_CUDA=OFF \
          -D CUDA_ARCH_BIN=7.5 .. && \
    make -j 6 && \
    make install

# Install ROS Noetic
RUN sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
RUN apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654
RUN apt-get update && \
    apt-get install -y --no-install-recommends ros-noetic-desktop-full
RUN apt-get install -y python3-vcstool python3-catkin-tools python3-osrf-pycommon ros-noetic-rqt* ros-noetic-libuvc-camera ros-noetic-usb-cam*
RUN apt-get install -y --no-install-recommends python3-rosdep 
RUN rosdep init && \
    rosdep fix-permissions && \
    rosdep update
RUN echo "source /opt/ros/noetic/setup.bash" >> ~/.bashrc

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
