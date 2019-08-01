#ifndef SRC_Tracker_HPP_
#define SRC_Tracker_HPP_

#define PI 3.141592653589793

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>
#include <cmath>
#include "fhog.hpp"

using namespace cv;
using namespace std;

class Tracker
{
public:
  Tracker(std::string kernel_type = "gaussian", std::string feature_type = "hog");
  void Init(cv::Mat image, cv::Rect rect_init);
  cv::Rect Update(cv::Mat image);
protected:
  void Learn(cv::Mat &patch, float lr);

  cv::Mat CreateGaussian1D(int n, float sigma);

  cv::Mat CreateGaussian2D(cv::Size sz, float sigma);

  cv::Mat GaussianShapedLabels(float sigma, cv::Size sz);

  cv::Mat CalculateHann(cv::Size sz);

  std::vector<cv::Mat> GetFeatures(cv::Mat patch);

  cv::Mat GaussianCorrelation(std::vector<cv::Mat> xf, std::vector<cv::Mat> yf);

  cv::Mat PolynomialCorrelation(std::vector<cv::Mat> xf, std::vector<cv::Mat> yf);

  cv::Mat LinearCorrelation(std::vector<cv::Mat> xf, std::vector<cv::Mat> yf);

  cv::Mat ComplexMul(const cv::Mat &x1, const cv::Mat &x2);

  cv::Mat ComplexDiv(const cv::Mat &x1, const cv::Mat &x2);
	 }

private:
  float padding_ = 1.5;
  float lambda_ = 1e-4;
  float output_sigma_factor_ = 0.1;
  bool features_hog_ = false;
  int features_hog_orientations_ = 9;
  bool features_gray_ = false;
  float kernel_sigma_ = 0.2;
  int kernel_poly_a_ = 1;
  int kernel_poly_b_ = 7;
  std::string kernel_type_;
  int cell_size_ = 1;
  float interp_factor_ = 0.075;
  cv::Rect result_rect_;
  cv::Point pos_;
  cv::Size target_sz_;
  bool resize_image_ = false;
  cv::Size window_sz_;
  cv::Mat yf_;
  cv::Mat cos_window_;
  std::vector<cv::Mat> model_xf_;
  cv::Mat model_alphaf_;
  FHoG f_hog_;
};

#endif /* SRC_Tracker_HPP_ */
