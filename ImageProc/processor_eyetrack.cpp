#include "singleton_factory.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "processor_facedetect.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>

class EyeTrackProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
    EyeTrackProcessor();
    virtual ~EyeTrackProcessor(){};
    void init(const char*); // Add image to paste
  private:
    void drawBorder( cv::Mat );
    void calcGradient( cv::Mat );
    cv::Rect getRightEyeRoI( cv::Mat);
    cv::Rect getLeftEyeRoI( cv::Mat);
    static void createOutput(cv::Mat&, cv::Mat&);
    std::unique_ptr<FaceDetectorProcessor> innerFaceDetector;
    // Parameters for eye location in units of face width,height
    double eyeWidth = 0.3;
    double eyeHeight = 0.3;
    double eyeX0 = 0.15;
    double eyeY0 = 0.25;
};

EyeTrackProcessor::EyeTrackProcessor() {
  // Use inner FaceDetector processor for finding faces in image
  innerFaceDetector = std::unique_ptr<FaceDetectorProcessor>{new FaceDetectorProcessor()};
}

void EyeTrackProcessor::init( const char* json ) {
}

cv::Mat EyeTrackProcessor::operator()(cv::Mat im) {
  // EyeTrack segmentation demo from the OpenCV documentation
  cv::cvtColor( im, im, CV_RGBA2BGR );

  std::vector<cv::Rect> faces;
  innerFaceDetector->detectFaces(im, faces);
  
  for ( size_t i=0; i < faces.size(); i++ ) {
    // Whole face RoI
    cv::Mat imFace = im( faces[i] );
    cv::Rect rightEye = getRightEyeRoI(imFace);
    // Person left eye (camera right)
    cv::Rect leftEye = getLeftEyeRoI(imFace);

    calcGradient(imFace(rightEye));
    calcGradient(imFace(leftEye));
    
    drawBorder(imFace(rightEye));
    drawBorder(imFace(leftEye));
  }

  // Display intermediate images for debugging
  // cv::Mat dest;
  // createOutput( markers, dest );
  cv::cvtColor( im, im, CV_BGR2RGBA );
  return im; 
}

void EyeTrackProcessor::drawBorder( cv::Mat im){
    cv::Rect border(0,0,im.cols,im.rows);
    cv::rectangle( im, border, cv::Scalar(0,255,0), 1);
}

void EyeTrackProcessor::calcGradient( cv::Mat im) {
  // Takes input BGR image and calculates gradient after smoothing
  // Remove noise by blurring with Gaussian
  
  cv::Mat imC;
  im.copyTo(imC);
  cv::GaussianBlur(imC, imC, cv::Size(3,3), 0,0, cv::BORDER_DEFAULT);
  cv::Mat grey(imC.size(), CV_8UC1);
  cv::cvtColor( imC, grey, CV_BGR2GRAY);
  
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  int ksize = CV_SCHARR;
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
  // return gradient;

  cv::Mat dest( gradient.size(), CV_8UC3);
  std::cout << "Gradient: " << gradient.channels() << 
               ", Dest: " << dest.channels() << std::endl;

  cv::cvtColor( gradient, dest, CV_GRAY2BGR);
  dest.copyTo(im);
}

cv::Rect EyeTrackProcessor::getRightEyeRoI( cv::Mat imFace ) {
    int faceWidth = imFace.cols;
    int faceHeight = imFace.rows;
    int w = std::round( faceWidth * eyeWidth);
    int h = std::round( faceHeight * eyeHeight);
    int x0 = std::round( faceWidth * eyeX0);
    int y0 = std::round( faceHeight * eyeY0);
    // Person right eye (camera left)
    cv::Rect roiRightEye(x0, y0, w, h);
    return roiRightEye;
}

cv::Rect EyeTrackProcessor::getLeftEyeRoI( cv::Mat imFace ) {
    int faceWidth = imFace.cols;
    int faceHeight = imFace.rows;
    int w = std::round( faceWidth * eyeWidth);
    int h = std::round( faceHeight * eyeHeight);
    int x0 = std::round( faceWidth * eyeX0);
    int y0 = std::round( faceHeight * eyeY0);
    // Person right eye (camera left)
    cv::Rect roiLeftEye(faceWidth - x0 - w, y0, w, h);
    return roiLeftEye;
}

void EyeTrackProcessor::createOutput( cv::Mat& src, cv::Mat& dest )
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
  auto scProcReg = ProcessorRegister<EyeTrackProcessor>("Eye Tracker");
}
