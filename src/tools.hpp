#pragma once

//#include <cv.h>
#include <math.h>

#ifndef _OPENCV_TOOLS_HPP_
#define _OPENCV_TOOLS_HPP_
#endif


namespace Tools{

//-------------------------------------------------------------------------------------
inline std::vector<double> PrecisionCalculate(std::vector<cv::Rect>groundtruth_rect,
                                       std::vector<cv::Rect>result_rect) {
  /*！
    calculate distance precision rate
    para:
      groundtruth_rect: vector of cv::Rect
      result_rect: vector of cv::Rect
    output:
      precisions: vector of double, each precision for different threshold
  ！*/
  int max_threshold = 50;// max threshold between groundtruth center and result center.
  std::vector<double> precisions(max_threshold + 1, 0);
  if (groundtruth_rect.size() != result_rect.size()) {
    int n = min(groundtruth_rect.size(), result_rect.size());
    groundtruth_rect.erase(groundtruth_rect.begin()+n, groundtruth_rect.end());
    result_rect.erase(result_rect.begin() + n, result_rect.end());
  }
  std::vector<double> distances;
  double distemp;
  for (unsigned int i = 0; i < result_rect.size(); ++i) {
    distemp = sqrt(double(pow((result_rect[i].x + result_rect[i].width / 2) -
              (groundtruth_rect[i].x + groundtruth_rect[i].width / 2), 2) +
                          pow((result_rect[i].y + result_rect[i].height / 2) -
              (groundtruth_rect[i].y + groundtruth_rect[i].height / 2), 2)));
    distances.push_back(distemp);
  }
  for (int i = 0; i <= max_threshold; ++i) {
    for (unsigned int j = 0; j < distances.size(); ++j) {
      if (distances[j] < double(i))
        precisions[i]++;
    }
    precisions[i] = precisions[i] / distances.size();
  }
  return precisions;
}
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
inline std::vector<cv::Rect> GetGroundtruth(std::string txt_file) {
  /*!
    param: 
      txt_file: groundtruth file path
    output: 
      vector: vector of cv::Rect object  
  !*/
  std::vector<cv::Rect> rect;
  ifstream gt;
  gt.open(txt_file.c_str());
  if (!gt.is_open())
    std::cout << "Ground truth file " << txt_file 
              << " can not be read" << std::endl;
  std::string line;
  int tmp1, tmp2, tmp3, tmp4;
  while (getline( gt, line)) {
    std::replace(line.begin(), line.end(), ',', ' ');
    stringstream ss;
    ss.str(line);
    ss >> tmp1 >> tmp2 >> tmp3 >> tmp4;
    rect.push_back( cv::Rect(--tmp1, --tmp2, tmp3, tmp4) ); //0-index
  }
  gt.close();
  return rect;
}
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
inline cv::Size FloorSizeScale(cv::Size sz, double scale_factor) {
	if (scale_factor > 0.9999 && scale_factor < 1.0001)
		return sz;
	return cv::Size(cvFloor(sz.width * scale_factor), 
		cvFloor(sz.height * scale_factor));
}
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
inline cv::Point FloorPointScale(cv::Point p, double scale_factor) {
    if (scale_factor > 0.9999 && scale_factor < 1.0001)
        return p; 
    return cv::Point(cvFloor(p.x * scale_factor), 
        cvFloor(p.y * scale_factor));
}
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
inline cv::Rect cutWindow(const cv::Rect& subwindow, const cv::Rect& limit){
    cv::Rect res;
    //right border
    if(subwindow.x + subwindow.width > limit.x + limit.width)
        res.width = limit.x + limit.width - subwindow.x;
    //bottom border
    if(subwindow.y + subwindow.height > limit.y + limit.height)
        res.height = limit.y + limit.height - subwindow.y;
    //left border
    if(subwindow.x < limit.x){
        res.width = subwindow.width - (limit.x - subwindow.x);
        res.x = limit.x;
    }
    //top border
    if(subwindow.y < limit.y){
        res.height = subwindow.height - (limit.y - subwindow.y);
    }
    if(res.width < 0) res.width = 0;
    if(res.height < 0) res.height = 0;
    return res;
}
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
inline cv::Rect getBorder(const cv::Rect& subwindow, const cv::Rect& limitBox){
    cv::Rect res;
    //left
    res.x = limitBox.x - subwindow.x;
    //top
    res.y = limitBox.y - subwindow.y;
    //right
    res.width = subwindow.width - (limitBox.x + limitBox.width);
    //bottom
    res.height = subwindow.height - (limitBox.y + limitBox.height);
    return res;
}
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
inline cv::Mat getSubwindow(const cv::Mat& frame, cv::Point centerCoor, cv::Size sz){
    cv::Rect limit(0, 0, frame.cols, frame.rows);
    cv::Rect subwindow(centerCoor.x-sz.width, centerCoor.y-sz.height, sz.width, sz.height);
    cv::Rect cutedWindow = cutWindow(subwindow, limit);
    cv::Rect border = getBorder(subwindow, cutedWindow);
    cv::Mat res(frame, cutedWindow);
    if(border != cv::Rect(0,0,0,0))
        cv::copyMakeBorder(res, res, border.y, border.height, border.x, border.width, cv::BORDER_CONSTANT);
    return res;
}
//-------------------------------------------------------------------------------------
}