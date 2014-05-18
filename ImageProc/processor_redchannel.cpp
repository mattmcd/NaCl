#include "singleton_factory.hpp"
#include <vector>

class SingleChannelProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
};

cv::Mat SingleChannelProcessor::operator()(cv::Mat im) {
  cv::Mat dest;
  // Split the matrix into its channels
  std::vector<cv::Mat> rgba;
  cv::split(im, rgba);
  // Blank matrix for Green and Blue channels
  cv::Mat blank = cv::Mat( im.size(), CV_8UC1, cv::Scalar(0));
  std::vector<cv::Mat> rgba_out;
  rgba_out.push_back( rgba[0] );
  rgba_out.push_back( rgba[0] );
  rgba_out.push_back( rgba[0] );
  rgba_out.push_back( rgba[3] );

  cv::merge( rgba_out, dest);
  return dest; 
}

namespace {
  auto scProcReg = ProcessorRegister<SingleChannelProcessor>("Red");
}

