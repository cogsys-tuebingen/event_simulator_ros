/* ROS node which uses the event simulator library to simulate events
 * given frames from a frame-based camera received via ROS messages.
 */

#include <cv_bridge/cv_bridge.h>
#include <event_simulator/DISOpticalFlowCalculator.h>
#include <event_simulator/DenseInterpolatedEventSimulator.h>
#include <event_simulator/DifferenceInterpolatedEventSimulator.h>
#include <event_simulator/FarnebackFlowCalculator.h>
#include <event_simulator/LKOpticalFlowCalculator.h>
#include <event_simulator/OpticalFlow.h>
#include <event_simulator/SparseInterpolatedEventSimulator.h>
#include <image_transport/image_transport.h>
#include <prophesee_event_msgs/EventArray.h>
#include <ros/ros.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/Image.h>
#include <std_msgs/Header.h>

#include <stdexcept>
#ifdef USE_CUDA
#include <event_simulator/CudaFarnebackFlowCalculator.h>
#include <event_simulator/CudaLKOpticalFlowCalculator.h>
#endif

/**
 * @brief Event simulator node to be used with ROS.
 */
class EventSimulatorNode {
 public:
  /**
   * @brief Constructor initializes the event simulator node depending on the settings.
   *
   * @param node_handle The ROS node handle
   * @param event_simulator_type Type of event simulator used
   * @param publish_events Flag indicating if events will be published or not
   * @param publish_event_frames Flag indicating if accumulated event frames will be published or noFlag indicating if accumulated event frames will be published or not   * @param c_pos [TODO:description]
   * @param c_neg Negative threshold
   * @param c_offset Offset (used to calculate the intermediate thresholds)
   * @param num_inter_frames Number of interpolated frames
   * @param div_factor Division factor (used to calculate some of the thresholds)
   */
  EventSimulatorNode(ros::NodeHandle &node_handle,
                     const std::string &event_simulator_type, const bool publish_events,
                     const bool publish_event_frames, const int c_pos = 20, const int c_neg = 20,
                     const int c_offset = 10, const int num_inter_frames = 10,
                     const int div_factor = 10)
      : image_transport_{node_handle},
        publish_events_{publish_events},
        publish_event_frames_{publish_event_frames},
        initialized_{false} {
    std::shared_ptr<SparseOpticalFlowCalculator> sparse_optical_flow = nullptr;
    std::shared_ptr<DenseOpticalFlowCalculator> dense_optical_flow = nullptr;

    if (event_simulator_type == "difference_cpu") {
      sparse_optical_flow = std::make_shared<LKOpticalFlowCalculator>();
      event_simulator_ = std::make_unique<DifferenceInterpolatedEventSimulator>(
          sparse_optical_flow, num_inter_frames, c_pos, c_neg, c_pos + c_offset,
          c_neg + c_offset);
    } else if (event_simulator_type == "difference_gpu") {
#ifdef USE_CUDA
      sparse_optical_flow = std::make_shared<CudaLKOpticalFlowCalculator>();
      event_simulator_ = std::make_unique<DifferenceInterpolatedEventSimulator>(
          sparse_optical_flow, num_inter_frames, c_pos, c_neg, c_pos + c_offset,
          c_neg + c_offset);
#else
      throw std::invalid_argument("CUDA not available");
#endif
    } else if (event_simulator_type == "sparse_cpu") {
      sparse_optical_flow = std::make_shared<LKOpticalFlowCalculator>();
      event_simulator_ = std::make_unique<SparseInterpolatedEventSimulator>(
          sparse_optical_flow, num_inter_frames, c_pos / 2, c_neg / 2);
    } else if (event_simulator_type == "sparce_gpu") {
#ifdef USE_CUDA
      sparse_optical_flow = std::make_shared<CudaLKOpticalFlowCalculator>();
      event_simulator_ = std::make_unique<SparseInterpolatedEventSimulator>(
          sparse_optical_flow, num_inter_frames, c_pos / 2, c_neg / 2);
#else
      throw std::invalid_argument("CUDA not available");
#endif
    } else if (event_simulator_type == "dense_farneback_cpu") {
      dense_optical_flow = std::make_shared<FarnebackFlowCalculator>();
      event_simulator_ = std::make_unique<DenseInterpolatedEventSimulator>(
          dense_optical_flow, num_inter_frames, c_pos / div_factor,
          c_neg / div_factor);
    } else if (event_simulator_type == "dense_farneback_gpu") {
#ifdef USE_CUDA
      dense_optical_flow = std::make_shared<CudaFarnebackFlowCalculator>();
      event_simulator_ = std::make_unique<DenseInterpolatedEventSimulator>(
          dense_optical_flow, num_inter_frames, c_pos / div_factor,
          c_neg / div_factor);
#else
      throw std::invalid_argument("CUDA not available");
#endif
    } else if (event_simulator_type == "dense_dis_lq") {
      dense_optical_flow = std::make_shared<DISOpticalFlowCalculator>(
          DISOpticalFlowQuality::LOW);
      event_simulator_ = std::make_unique<DenseInterpolatedEventSimulator>(
          dense_optical_flow, num_inter_frames, c_pos / div_factor,
          c_neg / div_factor);
    } else if (event_simulator_type == "dense_dis_hq") {
      dense_optical_flow = std::make_shared<DISOpticalFlowCalculator>(
          DISOpticalFlowQuality::HIGH);
      event_simulator_ = std::make_unique<DenseInterpolatedEventSimulator>(
          dense_optical_flow, num_inter_frames, c_pos / div_factor,
          c_neg / div_factor);
    } else {
      throw std::invalid_argument("Simulator type does not exist");
    }

    if (publish_event_frames_) {
      accumulated_events_pub_ =
          image_transport_.advertise("accumulated_events", 1);
    }

    if (publish_events_) {
      pub_info_ = node_handle.advertise<sensor_msgs::CameraInfo>(
          "/prophesee/camera_info", 1);

      events_publisher_ =
          node_handle.advertise<prophesee_event_msgs::EventArray>(
              "/prophesee/cd_events_buffer", 1000);
    }
  }

  /**
   * @brief ROS callback method executed when a frame is received.
   *        Simulates the events and published the events and/or the
   *        accumulated event frames.
   *
   * @param msg ROS message containing the frame
   */
  void imageCallback(const sensor_msgs::Image::ConstPtr &msg) {
    if (publish_events_ || publish_event_frames_) {
      try {
        cv_ptr_ = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);

        grey_frame_ = cv::Mat::zeros(msg->width, msg->height, CV_8UC3);
        cv::cvtColor(cv_ptr_->image, grey_frame_, cv::COLOR_BGR2GRAY);

        timestamp_ns_ = msg->header.stamp.nsec + msg->header.stamp.sec * 1e-9;
      } catch (cv_bridge::Exception &e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
      }

      if (!initialized_) {
        event_simulator_->setup(cv_ptr_->image.size());
        cam_info_msg_.width = msg->width;
        cam_info_msg_.height = msg->height;
        cam_info_msg_.header.frame_id = "PropheseeCamera_optical_frame";
        initialized_ = true;
      } else {
        int number_of_frames;

        if (publish_event_frames_) {
          auto out_frames = event_simulator_->getEventFrame(
              prev_frame_, grey_frame_, number_of_frames);

          for (const auto &frame : out_frames) {
            msg_ = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame)
                       .toImageMsg();
            msg_->header.stamp = msg->header.stamp;
            accumulated_events_pub_.publish(msg_);
          }
        }

        if (publish_events_) {
          auto events = event_simulator_->getEvents(
              prev_frame_, grey_frame_, prev_timestamp_ns_, timestamp_ns_,
              number_of_frames);

          event_array_msg_.header = msg->header;
          event_array_msg_.width = msg->width;
          event_array_msg_.height = msg->height;
          event_array_msg_.events.clear();
          event_array_msg_.events.reserve(events.size());

          prophesee_event_msgs::Event event_msg;
          for (const auto &event : events) {
            event_msg.x = event.x;
            event_msg.y = event.y;
            event_msg.ts.sec = 0.0;
            event_msg.ts.nsec = event.timestamp;
            event_msg.polarity = event.polarity;
            event_array_msg_.events.push_back(event_msg);
          }

          cam_info_msg_.header.stamp = msg->header.stamp;
          pub_info_.publish(cam_info_msg_);
          events_publisher_.publish(event_array_msg_);
        }
      }

      prev_frame_ = grey_frame_;
      prev_timestamp_ns_ = timestamp_ns_;
    }
  }

 private:
  /// ROS image transport
  image_transport::ImageTransport image_transport_;

  /// ROS publisher for the accumulated events
  image_transport::Publisher accumulated_events_pub_;

  /// Events publisher
  ros::Publisher events_publisher_;

  /// ROS event array message
  prophesee_event_msgs::EventArray event_array_msg_;

  /// Camera info
  sensor_msgs::CameraInfo cam_info_msg_;

  /// Info publisher
  ros::Publisher pub_info_;

  /// ROS image message
  sensor_msgs::ImagePtr msg_;
  
  /// ROS CV bridge
  cv_bridge::CvImagePtr cv_ptr_;

  /// Grey frame
  cv::Mat grey_frame_;

  /// Flag indicating if events should be published or not
  bool publish_events_;

  /// Flag indicating if accumulated event frames should be published or not
  bool publish_event_frames_;

  /// Pointer to the event simulator
  std::unique_ptr<EventSimulator> event_simulator_;

  /// Previous frame
  cv::Mat prev_frame_;

  /// Time stamp [ns]
  unsigned int timestamp_ns_;

  /// Previous time stamp [ns]
  unsigned int prev_timestamp_ns_;

  /// Flag indicating if the ROS node is initialized or not
  bool initialized_;
};

int main(int argc, char **argv) {
  ros::init(argc, argv, "event_simulator");
  ros::NodeHandle node_handle("~");

  std::string event_simulator_type;
  if (node_handle.param("type", event_simulator_type,
                        std::string("difference_cpu"))) {
    ROS_WARN_STREAM("Event simulator type: " << event_simulator_type);
  }

  bool publish_events;
  if (node_handle.param("publish_events", publish_events, true)) {
    ROS_WARN_STREAM("Publish events: " << publish_events);
  }
  bool publish_event_frames;
  if (node_handle.param("publish_event_frames", publish_event_frames, true)) {
    ROS_WARN_STREAM("Publish event frames: " << publish_event_frames);
  }

  EventSimulatorNode event_simulator_node(node_handle, event_simulator_type,
                                          publish_events, publish_event_frames);
  ros::Subscriber image_subscriber = node_handle.subscribe(
      "/usb_cam/image_raw", 10, &EventSimulatorNode::imageCallback,
      &event_simulator_node);

  ros::spin();
}
