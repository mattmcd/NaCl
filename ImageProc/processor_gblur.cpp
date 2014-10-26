#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class GBlurProcessor : public Processor {
  public:
    GBlurProcessor() : level(4) {};
    cv::Mat operator()(cv::Mat);
  private:
    int level;
};

cv::Mat GBlurProcessor::operator()(cv::Mat im) {
  cv::Mat dest;
  // for (auto i=0; i<level; i++) {
  //  cv::GaussianBlur(im,dest,cv::Size(5,5),0,0);
  //  im = dest;
  //}
  for (auto i=0; i<level; i++) {
    cv::pyrDown(im,dest);
    im = dest;
  }
  for (auto i=0; i<level; i++) {
    cv::pyrUp(im,dest);
    im = dest;
  }
  return dest;
}

namespace {
  auto diffProcReg = ProcessorRegister<GBlurProcessor>("Gaussian Blur");
}

