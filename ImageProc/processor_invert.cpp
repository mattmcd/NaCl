#include "singleton_factory.hpp"

class InvertProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
};

cv::Mat InvertProcessor::operator()(cv::Mat im) {
    cv::Mat dest;
    // Subtract the image from a constant 255 4-channel array
    cv::subtract( cv::Scalar(255, 255, 255, 255), im, dest );
    // Set the alpha channel back to fully opaque
    cv::add( cv::Scalar(0,0,0,255), dest, im );
    
    return im;
}

namespace {
  auto invertProcReg = ProcessorRegister<InvertProcessor>("Invert");
}
