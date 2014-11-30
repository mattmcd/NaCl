#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class ZBarProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
};

cv::Mat ZBarProcessor::operator()(cv::Mat im) {
  cv::Mat grey;
  cv::cvtColor( im, grey, CV_BGR2GRAY );
  cv::Mat gradX;
  cv::Scharr(grey, gradX, CV_32F, 1, 0);
  cv::Mat gradY;
  cv::Scharr(grey, gradY, CV_32F, 0, 1);
  cv::Mat gradient;
  cv::subtract( gradX, gradY, gradient);
  cv::convertScaleAbs(gradient, gradient);

  cv::blur(gradient, gradient, cv::Size(9,9));

  // Show intermediate images
  cv::Mat fullAlpha = cv::Mat( im.size(), CV_8UC1, cv::Scalar(255));
  std::vector<cv::Mat> rgba_out;
  rgba_out.push_back( gradient );
  rgba_out.push_back( gradient );
  rgba_out.push_back( gradient );
  rgba_out.push_back( fullAlpha );

  cv::Mat dest;
  cv::merge( rgba_out, dest);
  return dest; 
}

namespace {
  auto scProcReg = ProcessorRegister<ZBarProcessor>("Barcode Reader");
}

