/* Main application which uses the event simulator library to simulate events
 * given frames from a video. The accumulated event frames can be recorded 
 * and event statistics can be calculate.
 */

#include <event_simulator/DISOpticalFlowCalculator.h>
#include <event_simulator/DenseInterpolatedEventSimulator.h>
#include <event_simulator/DifferenceInterpolatedEventSimulator.h>
#include <event_simulator/FarnebackFlowCalculator.h>
#include <event_simulator/LKOpticalFlowCalculator.h>
#include <event_simulator/OpticalFlow.h>
#include <event_simulator/Player.h>
#include <event_simulator/SparseInterpolatedEventSimulator.h>

#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <string>
#ifdef USE_CUDA
#include <event_simulator/CudaFarnebackFlowCalculator.h>
#include <event_simulator/CudaLKOpticalFlowCalculator.h>
#endif

int main(int argc, const char *argv[]) {
  boost::program_options::options_description od{"Options"};
  od.add_options()("help,h", "Help screen")(
      "video", boost::program_options::value<std::string>()->default_value(""),
      "Path and filename")(
      "height", boost::program_options::value<int>()->default_value(0),
      "Height of the video frames")(
      "width", boost::program_options::value<int>()->default_value(0),
      "Width of the video frames")("statistics",
                                   boost::program_options::bool_switch(),
                                   "Get event statistics")(
      "record_video", boost::program_options::bool_switch(),
      "Record accumulated event frames")(
      "wait_time", boost::program_options::value<int>()->default_value(0),
      "How long each accumulated event frame should be displayed")(
      "type",
      boost::program_options::value<std::string>()->default_value(
          "difference_cpu"),
      "Event simulator type")(
      "x_res", boost::program_options::value<int>()->default_value(640),
      "Resolution X")("y_res",
                      boost::program_options::value<int>()->default_value(480),
                      "Resolution Y")(
      "c_pos", boost::program_options::value<int>()->default_value(20),
      "C positive")("c_neg",
                    boost::program_options::value<int>()->default_value(20),
                    "C negative")(
      "c_offset", boost::program_options::value<int>()->default_value(10),
      "C offset")("num_inter_frames",
                  boost::program_options::value<int>()->default_value(10),
                  "Number of interpolated inter frames");

  boost::program_options::variables_map vm;
  boost::program_options::store(
      boost::program_options::parse_command_line(argc, argv, od), vm);
  boost::program_options::notify(vm);

  auto video_path = vm["video"].as<std::string>();
  if (video_path.empty()) {
    throw std::invalid_argument("No video provided");
  }

  const auto width = vm["width"].as<int>();
  const auto height = vm["height"].as<int>();
  auto wait_time_ms = vm["wait_time"].as<int>();
  auto c_pos = vm["c_pos"].as<int>();
  auto c_neg = vm["c_neg"].as<int>();
  auto num_inter_frames = vm["num_inter_frames"].as<int>();
  auto c_offset = vm["c_offset"].as<int>();

  auto event_simulator_type = vm["type"].as<std::string>();
  std::shared_ptr<SparseOpticalFlowCalculator> sparse_optical_flow = nullptr;
  std::shared_ptr<DenseOpticalFlowCalculator> dense_optical_flow = nullptr;
  std::shared_ptr<EventSimulator> event_simulator;

  if (event_simulator_type == "difference_cpu") {
    sparse_optical_flow = std::make_shared<LKOpticalFlowCalculator>();
    event_simulator = std::make_unique<DifferenceInterpolatedEventSimulator>(
        sparse_optical_flow, num_inter_frames, c_pos, c_neg, c_pos + c_offset,
        c_neg + c_offset);
  } else if (event_simulator_type == "difference_gpu") {
#ifdef USE_CUDA
    sparse_optical_flow = std::make_shared<CudaLKOpticalFlowCalculator>();
    event_simulator = std::make_unique<DifferenceInterpolatedEventSimulator>(
        sparse_optical_flow, num_inter_frames, c_pos, c_neg, c_pos + c_offset,
        c_neg + c_offset);
#else
    throw std::invalid_argument("CUDA not available");
#endif
  } else if (event_simulator_type == "sparse_cpu") {
    sparse_optical_flow = std::make_shared<LKOpticalFlowCalculator>();
    event_simulator = std::make_unique<SparseInterpolatedEventSimulator>(
        sparse_optical_flow, num_inter_frames, c_pos / 2, c_neg / 2);
  } else if (event_simulator_type == "sparse_gpu") {
#ifdef USE_CUDA
    sparse_optical_flow = std::make_shared<CudaLKOpticalFlowCalculator>();
    event_simulator = std::make_unique<SparseInterpolatedEventSimulator>(
        sparse_optical_flow, num_inter_frames, c_pos / 2, c_neg / 2);
#else
    throw std::invalid_argument("CUDA not available");
#endif
  } else if (event_simulator_type == "dense_farneback_cpu") {
    dense_optical_flow = std::make_shared<FarnebackFlowCalculator>();
    event_simulator = std::make_unique<DenseInterpolatedEventSimulator>(
        dense_optical_flow, num_inter_frames, c_pos, c_neg);
  } else if (event_simulator_type == "dense_farneback_gpu") {
#ifdef USE_CUDA
    dense_optical_flow = std::make_shared<CudaFarnebackFlowCalculator>();
    event_simulator = std::make_unique<DenseInterpolatedEventSimulator>(
        dense_optical_flow, num_inter_frames, c_pos, c_neg);
#else
    throw std::invalid_argument("CUDA not available");
#endif
  } else if (event_simulator_type == "dense_dis_lq") {
    dense_optical_flow =
        std::make_shared<DISOpticalFlowCalculator>(DISOpticalFlowQuality::LOW);
    event_simulator = std::make_unique<DenseInterpolatedEventSimulator>(
        dense_optical_flow, num_inter_frames, c_pos, c_neg);
  } else if (event_simulator_type == "dense_dis_hq") {
    dense_optical_flow =
        std::make_shared<DISOpticalFlowCalculator>(DISOpticalFlowQuality::HIGH);
    event_simulator = std::make_unique<DenseInterpolatedEventSimulator>(
        dense_optical_flow, num_inter_frames, c_pos, c_neg);
  } else {
    throw std::invalid_argument("Simulator type does not exist");
  }

  auto event_statistics = vm["statistics"].as<bool>();
  auto record_video = vm["record_video"].as<bool>();
  OpenCVPlayer cv_player = OpenCVPlayer(event_simulator, wait_time_ms);
  cv_player.simulate(video_path, height, width, 1, event_statistics,
                     record_video);

  return 0;
}
