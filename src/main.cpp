#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "tracker.hpp"
#include "tools.hpp"

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "Usage:" 
              << argv[0] << " video_base_path[./Human3] Verbose[0/1]" << std::endl;
    return 0;
  }

  // read image files
  std::string video_base_path = argv[1];
  std::string pattern_jpg = video_base_path+ "/img/*.jpg";
  std::string pattern_png = video_base_path+ "/img/*.png";
  std::vector<cv::String> image_files;
  cv::glob(pattern_jpg, image_files);//read all image files to image_files
  if (image_files.size() == 0)
    cv::glob(pattern_png, image_files);
  if (image_files.size() == 0) {
    std::cout << "No image files[jpg png]" << std::endl;
    return 0;
  }

  // get groundtruth
  std::string txt_base_path = video_base_path + "/groundtruth_rect.txt";
  std::vector<cv::Rect> groundtruth_rect;
  groundtruth_rect = Tools::GetGroundtruth(txt_base_path);

  // set some parameters
  cv::Mat image;
  std::vector<cv::Rect> result_rect;
  int64 tic, toc;// time control
  double time = 0;
  bool show_visualization = argc == 3? atoi(argv[2]) : true;
  std::string kernel_type = "gaussian";//gaussian polynomial linear
  std::string feature_type = "hog";//hog gray

  // tracker object
  Tracker tracker(kernel_type, feature_type);

  // tracking part
  for(unsigned int frame = 0; frame < image_files.size(); ++frame) {
    image = cv::imread(image_files[frame]);
    tic = getTickCount();
    if (frame == 0) {
      tracker.Init(image, groundtruth_rect[0]);
      result_rect.push_back(groundtruth_rect[0]); //0-index
    } else {
      result_rect.push_back(tracker.Update(image));
    }
    toc = cv::getTickCount() - tic;
    time += toc;

    // tracking visualization
    if (show_visualization) {
      cv::putText(image, to_string(frame+1), cv::Point(20, 40), 6, 1,
                  cv::Scalar(0, 255, 255), 2);
      cv::rectangle(image, groundtruth_rect[frame], cv::Scalar(0, 255, 0), 2);
      cv::rectangle(image, result_rect[frame], cv::Scalar(0, 255, 255), 2);
      cv::imshow(video_base_path, image);
    
      char key = cv::waitKey(1);
      if(key == 27 || key == 'q' || key == 'Q')
        break;
    }
  }

  // compute fps
  time = time / double(getTickFrequency());
  double fps = double(result_rect.size()) / time;

  // compute precision
  std::vector<double> precisions = Tools::PrecisionCalculate(groundtruth_rect,
                                                      result_rect);
  printf("%12s - Precision (20px) : %1.3f, FPS : %4.2f\n",
         video_base_path.c_str(), precisions[20], fps);
  cv::destroyAllWindows();

  return 0;
}



