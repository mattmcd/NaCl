#include "singleton_factory.hpp"

namespace {
  auto invertReg = ObjectRegister<std::function<cv::Mat(cv::Mat)> >("Invert", 
  [](cv::Mat im){ 
      cv::rectangle(im, cv::Point(100,100), cv::Point(200,200), cv::Scalar(255,0,0), 1);
      return im; });
}

