#include "singleton_factory.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>

class FaceDetectorProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
  private:
    cv::CascadeClassifier face_cascade;
    bool xmlLoaded;
    static void createOutput(cv::Mat&, cv::Mat&);
};

cv::Mat FaceDetectorProcessor::operator()(cv::Mat im) {
  if (face_cascade.empty() )
    xmlLoaded = face_cascade.load("haarcascade_frontalface_alt.xml");
  if (!xmlLoaded) 
    return im; // Early exit
  
  // FaceDetector segmentation demo from the OpenCV documentation
  cv::Mat grey;
  cv::cvtColor( im, im, CV_RGBA2BGR );
  cv::cvtColor( im, grey, CV_BGR2GRAY );
  cv::equalizeHist( grey, grey );

  std::vector<cv::Rect> faces;
  face_cascade.detectMultiScale( grey, faces, 1.1, 2, 
    0 | CV_HAAR_SCALE_IMAGE, cv::Size(30,30));

  for ( size_t i=0; i < faces.size(); i++ ) {
    cv::rectangle( im, faces[i], cv::Scalar(0,255,0), 2);
  }

  // Display intermediate images for debugging
  // cv::Mat dest;
  // createOutput( markers, dest );
  cv::cvtColor( im, im, CV_BGR2RGBA );
  return im; 
}

void FaceDetectorProcessor::createOutput( cv::Mat& src, cv::Mat& dest )
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
  auto scProcReg = ProcessorRegister<FaceDetectorProcessor>("Face Detector");
}

