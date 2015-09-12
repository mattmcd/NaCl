#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>

// OpenCV HoughLine Derivative example from doc
// http://docs.opencv.org/doc/tutorials/imgtrans/sobel_derivatives/sobel_derivaties.html
class HoughLineProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
  private:
    static void createOutput(cv::Mat&, cv::Mat&);
};

cv::Mat HoughLineProcessor::operator()(cv::Mat im) {
 
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
  std::vector<cv::Vec2f> lines;
  int houghRho = 1; // pixel
  double houghTheta = CV_PI/180; //radians
  int houghThreshold = 100;
  int srn = 0;
  int stn = 0;
  cv::HoughLines( detected_edges, lines, houghRho, houghTheta, houghThreshold, srn, stn);

  // Display intermediate images for debugging
  cv::Mat dest;
  cv::cvtColor( detected_edges, dest, CV_GRAY2BGR);

  // Draw detected lines
  for( auto line : lines ) {
    float rho = line[0], theta = line[1];
    cv::Point pt1, pt2;
    double a = cos(theta), b = sin(theta);
    double x0 = a*rho, y0 = b*rho;
    pt1.x = cvRound(x0 + 1000*(-b));
    pt1.y = cvRound(y0 + 1000*(a));
    pt2.x = cvRound(x0 - 1000*(-b));
    pt2.y = cvRound(y0 - 1000*(a));
    cv::line(dest, pt1, pt2, cv::Scalar(0,255,0), 2, CV_AA);
  }

  cv::cvtColor( dest, dest, CV_BGR2RGBA );
  return dest; 

}

void HoughLineProcessor::createOutput( cv::Mat& src, cv::Mat& dest )
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
  auto scProcReg = ProcessorRegister<HoughLineProcessor>("Hough Line Transform");
}

