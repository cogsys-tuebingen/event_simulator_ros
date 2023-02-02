/* Main application to measure the run times of the different event simulators.
 * The number of iterations can be selected. The mean and std. deviation of the
 * run times will be reported.
 *
 * @note: Needs "config.yaml" which specifies the used settings
 */

#include <event_simulator/BasicDifferenceEventSimulator.h>
#include <event_simulator/BasicEventSimulator.h>
#include <event_simulator/DISOpticalFlowCalculator.h>
#include <event_simulator/DenseInterpolatedEventSimulator.h>
#include <event_simulator/DifferenceInterpolatedEventSimulator.h>
#include <event_simulator/FarnebackFlowCalculator.h>
#include <event_simulator/LKOpticalFlowCalculator.h>
#include <event_simulator/OpticalFlow.h>
#include <event_simulator/Player.h>
#include <event_simulator/SparseInterpolatedEventSimulator.h>
#include <yaml-cpp/yaml.h>

#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#ifdef USE_CUDA
#include <event_simulator/CudaFarnebackFlowCalculator.h>
#include <event_simulator/CudaLKOpticalFlowCalculator.h>
#endif

int main(int argc, const char* argv[]) {
  boost::program_options::options_description od{"Options"};
  od.add_options()("help,h", "Help screen")(
      "video", boost::program_options::value<std::string>()->default_value(""),
      "Path and filename")(
      "height", boost::program_options::value<int>()->default_value(0),
      "Height of the video frames")(
      "width", boost::program_options::value<int>()->default_value(0),
      "Width of the video frames")(
      "iterations", boost::program_options::value<int>()->default_value(10),
      "Number of iterations to calculate mean and std deviation of the run "
      "times")("num_frames",
               boost::program_options::value<int>()->default_value(0),
               "Number of frames in the video to use")(
      "statistics", boost::program_options::bool_switch(),
      "Get event statistics")("run_times",
                              boost::program_options::bool_switch(),
                              "Calculate run times (mean and std deviation")(
      "acc_events_frame",
      boost::program_options::value<int>()->default_value(0),
      "The frame number of the accumulated events frame")(
      "roi", boost::program_options::value<std::vector<int>>()->multitoken(),
      "The region of interest for the accumulated events frame")(
      "wait_time", boost::program_options::value<int>()->default_value(0),
      "How long each accumulated event frame should be displayed")(
      "type",
      boost::program_options::value<std::string>()->default_value(
          "difference_cpu"),
      "Event simulator type")(
      "c_pos_diff", boost::program_options::value<int>()->default_value(20),
      "C positive for the difference methods")(
      "c_neg_diff", boost::program_options::value<int>()->default_value(20),
      "C negative for the difference methods")(
      "c_pos_dense", boost::program_options::value<int>()->default_value(2),
      "C positive for the dense methods")(
      "c_neg_dense", boost::program_options::value<int>()->default_value(2),
      "C negative for the dense methods")(
      "c_pos_sparse", boost::program_options::value<int>()->default_value(10),
      "C positive for the sparse interpolated methods")(
      "c_neg_sparse", boost::program_options::value<int>()->default_value(10),
      "C negative for the sparse interpolated methods")(
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

  YAML::Node config = YAML::LoadFile("config.yaml");

  const int num_inter_frames = config["num_inter_frames"].as<int>();
  std::cout << "num_inter_frames: " << num_inter_frames << std::endl;
  const int c_pos_difference = config["basicdifference"]["c_pos"].as<int>();
  std::cout << "c_pos_difference: " << c_pos_difference << std::endl;
  const int c_neg_difference = config["basicdifference"]["c_neg"].as<int>();
  std::cout << "c_neg_difference: " << c_neg_difference << std::endl;
  const int c_pos_dense_dis_hq =
      config["dense"]["dis"]["hq"]["c_pos"].as<int>();
  std::cout << "c_pos_dense_dis_hq: " << c_pos_dense_dis_hq << std::endl;
  const int c_neg_dense_dis_hq =
      config["dense"]["dis"]["hq"]["c_neg"].as<int>();
  std::cout << "c_neg_dense_dis_hq: " << c_neg_dense_dis_hq << std::endl;
  const int c_pos_dense_dis_lq =
      config["dense"]["dis"]["lq"]["c_pos"].as<int>();
  std::cout << "c_pos_dense_dis_lq: " << c_pos_dense_dis_lq << std::endl;
  const int c_neg_dense_dis_lq =
      config["dense"]["dis"]["hq"]["c_neg"].as<int>();
  std::cout << "c_neg_dense_dis_lq: " << c_neg_dense_dis_lq << std::endl;
  const int c_pos_dense_fb_cpu =
      config["dense"]["farneback"]["cpu"]["c_pos"].as<int>();
  std::cout << "c_pos_dense_fb_cpu: " << c_pos_dense_fb_cpu << std::endl;
  const int c_neg_dense_fb_cpu =
      config["dense"]["farneback"]["cpu"]["c_neg"].as<int>();
  std::cout << "c_neg_dense_fb_cpu: " << c_neg_dense_fb_cpu << std::endl;
  const int c_pos_dense_fb_gpu =
      config["dense"]["farneback"]["gpu"]["c_pos"].as<int>();
  std::cout << "c_pos_dense_fb_gpu: " << c_pos_dense_fb_gpu << std::endl;
  const int c_neg_dense_fb_gpu =
      config["dense"]["farneback"]["gpu"]["c_neg"].as<int>();
  std::cout << "c_neg_dense_fb_gpu: " << c_neg_dense_fb_gpu << std::endl;
  const int c_pos_sparse_cpu = config["sparse"]["cpu"]["c_pos"].as<int>();
  std::cout << "c_pos_sparse_cpu: " << c_pos_sparse_cpu << std::endl;
  const int c_neg_sparse_cpu = config["sparse"]["cpu"]["c_neg"].as<int>();
  std::cout << "c_neg_sparse_cpu: " << c_neg_sparse_cpu << std::endl;
  const int c_pos_sparse_gpu = config["sparse"]["gpu"]["c_pos"].as<int>();
  std::cout << "c_pos_sparse_gpu: " << c_pos_sparse_gpu << std::endl;
  const int c_neg_sparse_gpu = config["sparse"]["gpu"]["c_neg"].as<int>();
  std::cout << "c_neg_sparse_gpu: " << c_neg_sparse_gpu << std::endl;
  const int c_pos_diff_cpu = config["difference"]["cpu"]["c_pos"].as<int>();
  std::cout << "c_pos_diff_cpu: " << c_pos_diff_cpu << std::endl;
  const int c_neg_diff_cpu = config["difference"]["cpu"]["c_neg"].as<int>();
  std::cout << "c_neg_diff_cpu: " << c_neg_diff_cpu << std::endl;
  const int c_pos_diff_gpu = config["difference"]["gpu"]["c_pos"].as<int>();
  std::cout << "c_pos_diff_gpu: " << c_pos_diff_gpu << std::endl;
  const int c_neg_diff_gpu = config["difference"]["gpu"]["c_neg"].as<int>();
  std::cout << "c_neg_diff_gpu: " << c_neg_diff_gpu << std::endl;
  const int c_offset = config["difference"]["c_offset"].as<int>();
  std::cout << "c_offset: " << c_offset << std::endl;

  std::shared_ptr<DenseOpticalFlowCalculator> dense_optical_flow_farneback_cpu =
      std::make_shared<FarnebackFlowCalculator>();
#ifdef USE_CUDA
  std::shared_ptr<DenseOpticalFlowCalculator> dense_optical_flow_farneback_gpu =
      std::make_shared<CudaFarnebackFlowCalculator>();
#endif

  std::shared_ptr<DenseOpticalFlowCalculator> dense_optical_dis_lq =
      std::make_shared<DISOpticalFlowCalculator>(DISOpticalFlowQuality::LOW);
  std::shared_ptr<DenseOpticalFlowCalculator> dense_optical_dis_hq =
      std::make_shared<DISOpticalFlowCalculator>(DISOpticalFlowQuality::HIGH);

  std::shared_ptr<SparseOpticalFlowCalculator> sparse_optical_flow_cpu =
      std::make_shared<LKOpticalFlowCalculator>();

#ifdef USE_CUDA
  std::shared_ptr<SparseOpticalFlowCalculator> sparse_optical_flow_gpu =
      std::make_shared<CudaLKOpticalFlowCalculator>();
#endif

  std::vector<std::shared_ptr<EventSimulator>> event_simulators = {
      std::make_shared<BasicEventSimulator>(),
      std::make_shared<BasicDifferenceEventSimulator>(c_pos_difference,
                                                      c_neg_difference),

      std::make_shared<DenseInterpolatedEventSimulator>(
          dense_optical_flow_farneback_cpu, num_inter_frames,
          c_pos_dense_fb_cpu, c_neg_dense_fb_cpu),

#ifdef USE_CUDA
      std::make_shared<DenseInterpolatedEventSimulator>(
          dense_optical_flow_farneback_gpu, num_inter_frames,
          c_pos_dense_fb_gpu, c_neg_dense_fb_gpu),
#endif

      std::make_shared<DenseInterpolatedEventSimulator>(
          dense_optical_dis_lq, num_inter_frames, c_pos_dense_dis_lq,
          c_neg_dense_dis_lq),

      std::make_shared<DenseInterpolatedEventSimulator>(
          dense_optical_dis_hq, num_inter_frames, c_pos_dense_dis_hq,
          c_neg_dense_dis_hq),

      std::make_unique<SparseInterpolatedEventSimulator>(
          sparse_optical_flow_cpu, num_inter_frames, c_pos_sparse_cpu,
          c_neg_sparse_cpu),

#ifdef USE_CUDA
      std::make_shared<SparseInterpolatedEventSimulator>(
          sparse_optical_flow_gpu, num_inter_frames, c_pos_sparse_gpu,
          c_neg_sparse_gpu),
#endif

      std::make_shared<DifferenceInterpolatedEventSimulator>(
          sparse_optical_flow_cpu, num_inter_frames, c_pos_diff_cpu,
          c_neg_diff_cpu, c_pos_diff_cpu + c_offset, c_neg_diff_cpu + c_offset),

#ifdef USE_CUDA
      std::make_shared<DifferenceInterpolatedEventSimulator>(
          sparse_optical_flow_gpu, num_inter_frames, c_pos_diff_gpu,
          c_neg_diff_gpu, c_pos_diff_gpu + c_offset, c_neg_diff_gpu + c_offset)
#endif
  };

  // Code for timing the renderers
  OpenCVPlayer cv_player = OpenCVPlayer(event_simulators.at(0), 0);

  if (!vm["roi"].empty()) {
    const auto& roi = vm["roi"].as<std::vector<int>>();
    cv_player.setROI(cv::Rect(roi.at(0), roi.at(1), roi.at(2), roi.at(3)));
    std::cout << "ROI: " << roi.at(0) << ", " << roi.at(1) << ", " << roi.at(2)
              << ", " << roi.at(3) << std::endl;
  }

  const auto width = vm["width"].as<int>();
  const auto height = vm["height"].as<int>();
  const auto iterations = vm["iterations"].as<int>();
  const auto wait_time_ms = vm["wait_time"].as<int>();
  const auto num_frames = vm["num_frames"].as<int>();

  const auto event_statistics = vm["statistics"].as<bool>();
  const auto calculate_run_times = vm["run_times"].as<bool>();
  const auto acc_events_frame_nr = vm["acc_events_frame"].as<int>();

  std::ofstream file;
  if (calculate_run_times) {
    file.open("run_times.txt");
  }

  for (auto simulator : event_simulators) {
    std::vector<double> run_times;

    for (std::size_t i = 0; i < iterations; ++i) {
      cv_player.setEventSimulator(simulator);

      if (calculate_run_times) {
        double run_time =
            cv_player.simulateTimed(video_path, height, width, 1, num_frames);

        run_times.emplace_back(run_time);
      } else {
        cv_player.simulate(video_path, height, width, 1, event_statistics);
      }

      if (i == 0 && acc_events_frame_nr > 0) {
        cv_player.saveSingleFrame(video_path, height, width,
                                  acc_events_frame_nr);
      }
    }

    if (calculate_run_times) {
      double sum = std::accumulate(run_times.begin(), run_times.end(), 0.0);
      double mean = sum / run_times.size();
      std::vector<double> diff(run_times.size());
      std::transform(run_times.begin(), run_times.end(), diff.begin(),
                     [mean](double x) { return x - mean; });
      double sq_sum =
          std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
      double stdev = std::sqrt(sq_sum / run_times.size());

      std::string line = simulator->getName() +
                         ": run time: mean: " + std::to_string(mean) +
                         "ms std. dev.: " + std::to_string(stdev) + " \n";
      std::cout << line;
      file << line;
    }
  }

  if (calculate_run_times) {
    file.close();
  }

  return 0;
}
