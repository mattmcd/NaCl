#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>

// OpenCV Laplacian Derivative example from doc
// http://docs.opencv.org/doc/tutorials/imgtrans/laplace_operator/laplace_operator.html
class LaplacianProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
  private:
    static void createOutput(cv::Mat&, cv::Mat&);
};

cv::Mat LaplacianProcessor::operator()(cv::Mat im) {
 
  cv::Mat grey;
  // Remove noise by blurring with Gaussian
  cv::GaussianBlur(im, im, cv::Size(3,3), 0,0, cv::BORDER_DEFAULT);
  cv::cvtColor( im, grey, CV_BGR2GRAY );
  
  int kernel_size = 3;
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  cv::Mat lap;
  cv::Laplacian(grey, lap, ddepth, kernel_size, scale, delta, cv::BORDER_DEFAULT);

  // Scaled abs laplacian
  cv::Mat absLap;

  cv::convertScaleAbs(lap, absLap);

  // Display intermediate images for debugging
  cv::Mat dest;
  createOutput( absLap, dest );
  return dest; 
}

void LaplacianProcessor::createOutput( cv::Mat& src, cv::Mat& dest )
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
  auto scProcReg = ProcessorRegister<LaplacianProcessor>("Laplacian");
}

