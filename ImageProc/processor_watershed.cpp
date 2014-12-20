#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>

class WatershedProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
  private:
    static void createOutput(cv::Mat&, cv::Mat&);
};

cv::Mat WatershedProcessor::operator()(cv::Mat im) {
  // Watershed segmentation demo from the OpenCV documentation
  cv::Mat grey;
  cv::cvtColor( im, im, CV_RGBA2BGR );
  cv::cvtColor( im, grey, CV_BGR2GRAY );

  // We look for dark objects on a light background, as the original demo
  // was designed for finding coins on a table.  The THRESH_OTSU flag makes
  // OpenCV choose the optimum threshold levels for a given image.
  cv::Mat thresh;
  cv::threshold(grey, thresh, 0, 255, 
    cv::THRESH_BINARY_INV + cv::THRESH_OTSU );
  
  // Open image to remove noise
  cv::Mat kernel = cv::getStructuringElement( 
    cv::MORPH_RECT, cv::Size(3,3));
  cv::Mat opening;
  cv::morphologyEx( thresh, opening, cv::MORPH_OPEN, 
    kernel, cv::Point(-1,1), 2);

  // Next we want to determine where the boundary between foreground and
  // background lies.  

  // Sure background area.  This is created by dilating the region we've
  // already highlighted.  Any point that is still 0 must be part of the
  // background.  Any highlighted point could be boundary or foreground.
  cv::Mat sureBg;
  cv::dilate( opening, sureBg, kernel, cv::Point(-1,1), 3);

  // Sure foreground area.  We apply a distance transform to give each
  // point a value that is its distance from the nearest 0.  This means
  // that points near the boundary of the thresholded image get a low
  // value, while ones further from the boundary get a higher value.
  cv::Mat distTfm;
  cv::distanceTransform( opening, distTfm, CV_DIST_L2, 5);
  cv::Mat sureFg;
  double minVal;
  double maxVal;
  cv::Point minLoc;
  cv::Point maxLoc;
  // Thresholding the distance transform image high lights areas far from
  // any boundary i.e. sure foreground features
  cv::minMaxLoc( distTfm, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
  cv::threshold( distTfm, sureFg, 0.7*maxVal, 255, 0);

  // Convert to 8 bit unsigned for further processing
  sureFg.convertTo( sureFg, CV_8U);

  // Unknown regions - parts of the sure background excluding areas we know
  // are the foreground regions.
  cv::Mat unknown;
  cv::subtract( sureBg, sureFg, unknown );

  // Now need to create a mask for the watershed algorithm.  We want to set
  // each foreground object to have a unique index, and give the background
  // another index.  The region we are unsure of gets labelled with 0.
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours( sureFg, contours, hierarchy, cv::RETR_CCOMP, 
    cv::CHAIN_APPROX_SIMPLE );

  // Go through each foreground object and draw it to the marker matrix as
  // a differet positive integer.
  cv::Mat markers( sureFg.size(), CV_32S );
  int idx = 0;
  int compCount = 0;
  for (; idx >= 0; idx = hierarchy[idx][0], compCount++ ) {
    cv::drawContours( markers, contours, idx, cv::Scalar(compCount+1), 
    CV_FILLED, 8, hierarchy, INT_MAX );
  }
  // Set the marker for the background to be a different positive integer
  cv::add( markers, cv::Scalar(1), markers); // Non-zero background
  // Set all the unknown areas to 0.
  markers.setTo( cv::Scalar(0), unknown == 255 );

  // Watershed sets all of the markers == 0 points to the closest labelled
  // region, and marks the boundaries with a -1.
  cv::watershed( im, markers );
  // Highlight the boundaries on the original image
  im.setTo( cv::Scalar(0,0,255), markers == -1);

  // Used for displaying the final marker matrix
  // markers.convertTo( markers, CV_8U);

  // Display intermediate images for debugging
  // cv::Mat dest;
  // createOutput( markers, dest );
  cv::cvtColor( im, im, CV_BGR2RGBA );
  return im; 
}

void WatershedProcessor::createOutput( cv::Mat& src, cv::Mat& dest )
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
  auto scProcReg = ProcessorRegister<WatershedProcessor>("Watershed Segment");
}

