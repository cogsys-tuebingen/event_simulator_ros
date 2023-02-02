# Real-time event simulation with frame-based cameras (ROS1 wrapper)

This repository contains a ROS1 wrapper for the [real-time event simulator library](https://github.com/cogsys-tuebingen/event_simulator).

#### Citing

If you use this code in an academic context, please cite the following publication:

Paper: [Real-time event simulation with frame-based cameras](https://arxiv.org/pdf/2209.04634.pdf)

Video: [YouTube](https://www.youtube.com/@ZellTuebingen)

```
@inproceedings{Ziegler2022icra,
	title = {Real-time event simulation with frame-based cameras},
	booktitle = {2023 {International} {Conference} on {Robotics} and {Automation} ({ICRA})},
	publisher = {IEEE},
	author = {Ziegler, Andreas and Teigland, Daniel and Tebbe, Jonas and Gossard, Thomas and Zell, Andreas},
	month = {may},
	year = {2023},
}
```

## Using the code

### Software requirements

This code has been tested on Ubuntu 20.04 with ROS noetic.

Dependencies:
- OpenCV
- ROS Noetic
  - roscpp
  - prophesee_event_msgs
  - image_transport
  - cv_bridge
  - [event_simulator](https://github.com/cogsys-tuebingen/event_simulator)

### How to build

1. Source ROS `source /opt/ros/noetic/setup.bash`

2. Create a ROS workspace `mkdir -p ~/event-simulator_ws/src`

3. Clone repositories:
```
git clone https://github.com/cogsys-tuebingen/event_simulator.git src/event_simulator
cd https://github.com/cogsys-tuebingen/event_simulator_ros.git src/event_simulator
git submodule update --init --recursive
cd ../..
git clone src/event_simulator_ros
git clone https://github.com/prophesee-ai/prophesee_ros_wrapper.git 
cp -r prophesee_ros_wrapper/prophesee_event_msgs src/
rm -rf prophesee_ros_wrapper
git clone https://github.com/ros-drivers/usb_cam.git usb_cam
```

4. Build:
```
catkin config -DCMAKE_BUILD_TYPE=RelWithDebInfo`
catkin build
```

5. Run:
```
source devel/setup.bash
rosrun event_simulator_ros event_simulator_ros
```

### Parameters

- ``type``: The event simulator type (possible options are `difference_cpu`, `difference_gpu`,
  `sparse_cpu`, `sparse_gpu`, `dense_farneback_cpu`, `dense_farneback_gpu`, `dense_dis_lq`,
  `dense_dis_hq`)
- ``publish_events``: Set to `True` to publish the event stream
- ``publish_event_frames``: Set to `True` to publish the accumulated event frames

## Docker

In the `docker` folder, you will find `Dockerfiles` for setups with and without CUDA. When building the docker image, you will need `metavision.list` which you can get from Prophesee [here](https://www.prophesee.ai/metavision-intelligence-sdk-download/).

## Analysis

In the `scripts` folder, you can find a couple of helper scripts for the statistics experiments.

## Config

A default config for the timing measurement can be found in the `config` folder.

## License

This software is issued under the Apache License Version 2.0.
