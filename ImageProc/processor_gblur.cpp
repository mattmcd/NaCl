#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

class GBlurProcessor : public Processor {
  public:
    GBlurProcessor() : level(4) {};
    GBlurProcessor(int level_) : level(level_) {};
    cv::Mat operator()(cv::Mat);
  private:
    int level;
};

cv::Mat GBlurProcessor::operator()(cv::Mat im) {
  cv::Mat dest;
  const int in_rows = im.rows;
  const int in_cols = im.cols;
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

  cv::Rect roi(0,0,in_cols,in_rows);
  cv::Mat out;
  dest(roi).copyTo(out);

  return out;
}

namespace {
  auto diffProcReg = ProcessorRegister<GBlurProcessor>("Gaussian Blur");
}

