#include "singleton_factory.hpp"
#include <vector>

class DiffProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
  private:
    cv::Mat prevFrame;
};

cv::Mat DiffProcessor::operator()(cv::Mat im) {
  if ( prevFrame.empty() ) {
    prevFrame = im.clone();
  } else {
    cv::Mat dest;
    cv::absdiff( im, prevFrame, dest);
    prevFrame = im.clone(); // NB: need to clone image not just copy ref
    // Set alpha channel
    cv::add( cv::Scalar(0,0,0,255), dest, im );
  }
  return im;
}

namespace {
  auto diffProcReg = ProcessorRegister<DiffProcessor>("Diff");
}

