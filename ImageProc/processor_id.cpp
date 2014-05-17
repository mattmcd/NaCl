#include "processor.hpp"

class IdentityProcessor : public Processor {
  public:
    virtual cv::Mat operator()(cv::Mat);
};

cv::Mat IdentityProcessor::operator()(cv::Mat im) {
  return im;
}

namespace {
  auto idProcReg = ProcessorRegister<IdentityProcessor>("Id");
}

namespace {
  auto idReg = ObjectRegister<std::function<cv::Mat(cv::Mat)> >("Id", 
    [](cv::Mat im){ return im; });
}

