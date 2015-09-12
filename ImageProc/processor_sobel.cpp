#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>

// OpenCV Sobel Derivative example from doc
// http://docs.opencv.org/doc/tutorials/imgtrans/sobel_derivatives/sobel_derivaties.html
class SobelProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
  private:
    static void createOutput(cv::Mat&, cv::Mat&);
};

cv::Mat SobelProcessor::operator()(cv::Mat im) {
 
  cv::Mat grey;
  cv::GaussianBlur(im, im, cv::Size(3,3), 0,0, cv::BORDER_DEFAULT);
  cv::cvtColor( im, grey, CV_BGR2GRAY );
  
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  cv::Mat gradX;
  cv::Sobel(grey, gradX, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
  cv::Mat gradY;
  cv::Sobel(grey, gradY, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);

  // Scaled abs gradients
  cv::Mat absGradX, absGradY;

  cv::convertScaleAbs(gradX, absGradX);
  cv::convertScaleAbs(gradY, absGradY);

  cv::Mat gradient;
  cv::addWeighted(absGradX, 0.5, absGradY, 0.5, 0, gradient);

  // Display intermediate images for debugging
  cv::Mat dest;
  createOutput( gradient, dest );
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

void SobelProcessor::createOutput( cv::Mat& src, cv::Mat& dest )
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
  auto scProcReg = ProcessorRegister<SobelProcessor>("Sobel Derivative");
}

