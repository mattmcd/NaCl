#include "singleton_factory.hpp"

class IdentityProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
};

cv::Mat IdentityProcessor::operator()(cv::Mat im) {
  return im;
}

namespace {
  auto idProcReg = ProcessorRegister<IdentityProcessor>("Id");
}
