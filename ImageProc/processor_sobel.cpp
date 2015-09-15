#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>
#include "include/rapidjson/document.h"

// OpenCV Sobel Derivative example from doc
// http://docs.opencv.org/doc/tutorials/imgtrans/sobel_derivatives/sobel_derivaties.html
class SobelProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
    void init(const char*);
    std::string getParameters();
  private:
    static void createOutput(cv::Mat&, cv::Mat&);
    int ksize = CV_SCHARR;
};

cv::Mat SobelProcessor::operator()(cv::Mat im) {
 
  cv::Mat grey;
  // Remove noise by blurring with Gaussian
  cv::GaussianBlur(im, im, cv::Size(3,3), 0,0, cv::BORDER_DEFAULT);
  cv::cvtColor( im, grey, CV_BGR2GRAY );
  
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  cv::Mat gradX;
  cv::Sobel(grey, gradX, ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
  cv::Mat gradY;
  cv::Sobel(grey, gradY, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);

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
}

void SobelProcessor::init(const char* json) {
  // Read json string to set properties
  using namespace rapidjson;
  Document document;
  document.Parse(json);
  assert(document.IsObject());
  assert(document.HasMember("ksize"));
  ksize = document["ksize"].GetInt();
}

std::string SobelProcessor::getParameters() {
  std::ostringstream os;
  os <<  "{\"ksize\":" << ksize << "}";
  return os.str();
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

