#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>

class BarcodeDetectorProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
  private:
    static void createOutput(cv::Mat&, cv::Mat&);
};

cv::Mat BarcodeDetectorProcessor::operator()(cv::Mat im) {
  cv::Mat grey;
  cv::cvtColor( im, grey, CV_BGR2GRAY );
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
  cv::threshold(gradient, thresh, 225, 255, cv::THRESH_BINARY );

  // Close image to convert barcode lines into filled rectangle
  cv::Mat kernel = cv::getStructuringElement( 
    cv::MORPH_RECT, cv::Size(21,7));
  cv::Mat closed;
  cv::morphologyEx( thresh, closed, cv::MORPH_CLOSE, kernel);

  // Remove small blobs
  cv::erode( closed, closed, cv::Mat(), cv::Point(-1,1), 4);
  cv::dilate( closed, closed, cv::Mat(), cv::Point(-1,1), 4);

  // Find largest area
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours( closed, contours, cv::RETR_EXTERNAL, 
    cv::CHAIN_APPROX_SIMPLE );

  std::sort(contours.begin(), contours.end(), 
    [](const std::vector<cv::Point> a, const std::vector<cv::Point>b) { 
        return (cv::contourArea(a) > cv::contourArea(b)); });
 
  cv::RotatedRect rect = cv::minAreaRect( contours[0]);
  cv::Point2f vertices[4];
  rect.points( vertices );
  for (int i=0; i<4; i++) {
    cv::line( im, vertices[i], vertices[(i+1)%4], 
      cv::Scalar(0,255,0,255), 3);
  }

  // cv::drawContours( im, outContour, -1, cv::Scalar(0,255,0), 3);

  // Display intermediate images for debugging
  // cv::Mat dest;
  // createOutput( closed, dest );
  return im; 
}

void BarcodeDetectorProcessor::createOutput( cv::Mat& src, cv::Mat& dest )
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
  auto scProcReg = ProcessorRegister<BarcodeDetectorProcessor>("Barcode Detector");
}

