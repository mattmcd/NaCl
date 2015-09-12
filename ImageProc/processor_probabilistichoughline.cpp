#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>

// OpenCV ProbabilisticHoughLine Derivative example from doc
// http://docs.opencv.org/doc/tutorials/imgtrans/sobel_derivatives/sobel_derivaties.html
class ProbabilisticHoughLineProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
  private:
    static void createOutput(cv::Mat&, cv::Mat&);
};

cv::Mat ProbabilisticHoughLineProcessor::operator()(cv::Mat im) {
 
  cv::Mat grey;
  // Remove noise by blurring with Gaussian
  cv::GaussianBlur(im, im, cv::Size(3,3), 0,0, cv::BORDER_DEFAULT);

  cv::cvtColor( im, grey, CV_BGR2GRAY );
  
  // Canny edge detector
  int lowThreshold = 50;
  int ratio = 4;
  int kernel_size = 3;
  cv::Mat detected_edges;
  cv::Canny( grey, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);

  // Hough line transform
  std::vector<cv::Vec4i> lines;
  int houghRho = 1; // pixel
  double houghTheta = CV_PI/180; //radians
  int houghThreshold = 50;
  int minLineLength = 50;
  int maxLineGap = 10;
  cv::HoughLinesP( detected_edges, lines, houghRho, houghTheta, 
    houghThreshold, minLineLength, maxLineGap);

  cv::Mat dest;
  cv::cvtColor( detected_edges, dest, CV_GRAY2BGR);

  // Draw detected lines
  for( auto line : lines ) {
    cv::line(dest, cv::Point(line[0],line[1]), 
      cv::Point(line[2], line[3]), cv::Scalar(0,255,0), 2, CV_AA);
  }

  cv::cvtColor( dest, dest, CV_BGR2RGBA );
  return dest; 
}

void ProbabilisticHoughLineProcessor::createOutput( cv::Mat& src, cv::Mat& dest )
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
  auto scProcReg = ProcessorRegister<ProbabilisticHoughLineProcessor>(
    "Probabilistic Hough Line Transform");
}

