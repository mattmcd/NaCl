#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>

class SkinDetectProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
  private:
    static void createOutput(cv::Mat&, cv::Mat&);
};

cv::Mat SkinDetectProcessor::operator()(cv::Mat im) {
  // SkinDetect segmentation demo from the OpenCV documentation
  cv::Mat converted;
  cv::cvtColor( im, im, CV_RGBA2BGR );
  cv::cvtColor( im, converted, CV_BGR2HSV );

  cv::Mat thresh;
  // Skin Cluster in YCbCr space from 'Explicit Image Detection using 
  // YCbCr Space Color Model as Skin Detection' by Basilo, et al.
  // cv::Scalar colorLow {80, 135, 85};
  // cv::Scalar colorHigh {255, 180, 135};

  // HSV skin range from PyImageSearch 2014-08-18 blog post 
  cv::Scalar colorLow {0, 48, 80};
  cv::Scalar colorHigh {20, 255, 255};

  cv::inRange(converted, colorLow, colorHigh, thresh);
  
  // Open image to remove noise
  cv::Mat kernel = cv::getStructuringElement( 
    cv::MORPH_ELLIPSE, cv::Size(3,3));
  cv::Mat skinMask;
  cv::morphologyEx( thresh, skinMask, cv::MORPH_OPEN, 
    kernel, cv::Point(-1,1), 2);
  cv::GaussianBlur(skinMask, skinMask, cv::Size(3,3), 0, 0);


  cv::Mat out;
  cv::bitwise_and(im, im, out, skinMask);

  // Display intermediate images for debugging
  // cv::Mat dest;
  // createOutput( markers, dest );
  cv::cvtColor( out, out, CV_BGR2RGBA );
  return out; 
}

void SkinDetectProcessor::createOutput( cv::Mat& src, cv::Mat& dest )
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
  auto scProcReg = ProcessorRegister<SkinDetectProcessor>("Skin Detector");
}

