#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>

class CartoonProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
  private:
    static void createOutput(cv::Mat&, cv::Mat&);
};

cv::Mat CartoonProcessor::operator()(cv::Mat im) {
  cv::Mat grey;
  cv::cvtColor( im, grey, CV_BGR2GRAY );
  // Get rid of speckle first by median filter
  const int MEDIAN_KERNEL_SIZE = 9;
  cv::medianBlur(grey, grey, MEDIAN_KERNEL_SIZE);
  cv::Mat gradX;
  cv::Scharr(grey, gradX, CV_32F, 1, 0);
  cv::Mat gradY;
  cv::Scharr(grey, gradY, CV_32F, 0, 1);
  cv::Mat gradient;
  cv::subtract( gradX, gradY, gradient);
  cv::convertScaleAbs(gradient, gradient);

  cv::Mat blurred;
  cv::blur(gradient, blurred, cv::Size(9,9));

  cv::Mat thresh;
  cv::threshold(gradient, thresh, 150, 255, cv::THRESH_BINARY_INV );

  // Display intermediate images for debugging
  cv::Mat dest;
  createOutput( thresh, dest );
  return dest; 

  /* 
  cv::Mat segmented, gray, edges, edgesBgr;
  cv::pyrMeanShiftFiltering(im, segmented, 15, 40);
  cv::cvtColor(segmented, gray, CV_BGR2GRAY);
  cv::Canny(gray, edges, 150, 150);
  cv::cvtColor(edges, edgesBgr, CV_GRAY2BGR);
  cv::Mat result = segmented - edgesBgr;
  return result;
  */
}

void CartoonProcessor::createOutput( cv::Mat& src, cv::Mat& dest )
{
  // Show intermediate images - need to expand CV_8UC1 to RGBA
  cv::Mat fullAlpha = cv::Mat( src.size(), CV_8UC1, cv::Scalar(255));
  std::vector<cv::Mat> rgba_out;
  rgba_out.push_back( src );
  rgba_out.push_back( src );
  rgba_out.push_back( src );
  rgba_out.push_back( fullAlpha );

  cv::merge( rgba_out, dest);
}

namespace {
  auto scProcReg = ProcessorRegister<CartoonProcessor>("Cartoon");
}

