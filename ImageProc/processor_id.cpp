#include "singleton_factory.hpp"

namespace {
  auto circleReg = ObjectRegister<std::function<cv::Mat(cv::Mat)> >("Id", 
    [](cv::Mat im){ return im; });
}

