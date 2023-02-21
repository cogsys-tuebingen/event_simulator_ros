cmake_minimum_required(VERSION 3.0.2)
project(event_simulator_ros)

find_package(event_simulator REQUIRED)

find_package(
  OpenCV
  COMPONENTS core highgui
  REQUIRED)

find_package(yaml-cpp REQUIRED)

# Find catkin macros and libraries if COMPONENTS list like find_package(catkin
# REQUIRED COMPONENTS xyz) is used, also find other catkin packages
find_package(
  catkin
  REQUIRED
  roscpp
  prophesee_event_msgs
  image_transport
  cv_bridge
  event_simulator)

# System dependencies are found with CMake's conventions find_package(Boost
# REQUIRED COMPONENTS system)

# Uncomment this if the package has a setup.py. This macro ensures modules and
# global scripts declared therein get installed See
# http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

catkin_package(
  INCLUDE_DIRS
  LIBRARIES
  ${PROJECT_NAME}
  CATKIN_DEPENDS
  roscpp)

add_executable(${PROJECT_NAME} src/event_simulator_ros.cpp )

# Specify libraries to link a library or executable target against
# target_link_libraries(${PROJECT_NAME}_node ${catkin_LIBRARIES} )
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES}
                      event_simulator::event_simulator)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${catkin_INCLUDE_DIRS})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
          -pipe
          -march=native
          -Wall
          -Wextra
          $<$<CONFIG:Release>:-O3>>
          $<$<CONFIG:Debug>:-Og
          -g
          -ggdb3
          >>
          $<$<CXX_COMPILER_ID:MSVC>:
          $<$<CONFIG:Debug>:/Od
          /Wall
          /Zi>>)

add_executable(event_simulator_video src/event_simulator_video.cpp)

# Specify libraries to link a library or executable target against
# target_link_libraries(${PROJECT_NAME}_node ${catkin_LIBRARIES} )
target_link_libraries(event_simulator_video ${catkin_LIBRARIES}
                      event_simulator::event_simulator)

target_include_directories(
  event_simulator_video
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${catkin_INCLUDE_DIRS})

target_compile_features(event_simulator_video PUBLIC cxx_std_17)

target_compile_options(
  event_simulator_video
  PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
          -pipe
          -march=native
          -Wall
          -Wextra
          $<$<CONFIG:Release>:-O3>>
          $<$<CONFIG:Debug>:-Og
          -g
          -ggdb3
          >>
          $<$<CXX_COMPILER_ID:MSVC>:
          $<$<CONFIG:Debug>:/Od
          /Wall
          /Zi>>)

add_executable(event_simulator_timings src/event_simulator_timings.cpp)

# Specify libraries to link a library or executable target against
# target_link_libraries(${PROJECT_NAME}_node ${catkin_LIBRARIES} )
target_link_libraries(event_simulator_timings ${catkin_LIBRARIES}
                      event_simulator::event_simulator yaml-cpp)

target_include_directories(
  event_simulator_timings
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${catkin_INCLUDE_DIRS})

target_compile_features(event_simulator_timings PUBLIC cxx_std_17)

target_compile_options(
  event_simulator_timings
  PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
          -pipe
          -march=native
          -Wall
          -Wextra
          $<$<CONFIG:Release>:-O3>>
          $<$<CONFIG:Debug>:-Og
          -g
          -ggdb3
          >>
          $<$<CXX_COMPILER_ID:MSVC>:
          $<$<CONFIG:Debug>:/Od
          /Wall
          /Zi>>)
