#include "singleton_factory.hpp"

namespace {
  auto idReg = ObjectRegister<std::function<cv::Mat(cv::Mat)> >("Id", 
    [](cv::Mat im){ return im; });
}

