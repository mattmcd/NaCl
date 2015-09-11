#include "singleton_factory.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "processor_facedetect.hpp"
#include <algorithm>
#include <iostream>

class SmileyProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
    SmileyProcessor();
    virtual ~SmileyProcessor(){};
    void init(cv::Mat); // Add image to paste
  private:
    cv::Mat smiley;
    static void createOutput(cv::Mat&, cv::Mat&);
    std::unique_ptr<FaceDetectorProcessor> innerFaceDetector;
};

SmileyProcessor::SmileyProcessor() {
  // Use inner FaceDetector processor for finding faces in image
  innerFaceDetector = std::unique_ptr<FaceDetectorProcessor>{new FaceDetectorProcessor()};
}

void SmileyProcessor::init( cv::Mat im ) {
  cv::cvtColor( im, im, CV_RGBA2BGR );
  smiley = im;
}

cv::Mat SmileyProcessor::operator()(cv::Mat im) {
  // Smiley segmentation demo from the OpenCV documentation
  cv::cvtColor( im, im, CV_RGBA2BGR );

  std::vector<cv::Rect> faces;
  innerFaceDetector->detectFaces(im, faces);
  
  for ( size_t i=0; i < faces.size(); i++ ) {
    cv::Mat tmp( faces[i].size(), CV_8UC4 );
    cv::resize( smiley, tmp, faces[i].size());
    cv::Mat roi = im( faces[i] );    
    cv::Mat mask( tmp.size(), CV_8UC1 );
    cv::compare( tmp, cv::Scalar(100,100,100), mask, cv::CMP_NE);
    tmp.copyTo( roi, mask );
    // cv::rectangle( im, faces[i], cv::Scalar(0,255,0), 2);
  }

  // Display intermediate images for debugging
  // cv::Mat dest;
  // createOutput( markers, dest );
  cv::cvtColor( im, im, CV_BGR2RGBA );
  return im; 
}

void SmileyProcessor::createOutput( cv::Mat& src, cv::Mat& dest )
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
  auto scProcReg = ProcessorRegister<SmileyProcessor>("Smiley!");
}
