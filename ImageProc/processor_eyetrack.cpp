#include "singleton_factory.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "processor_facedetect.hpp"
#include "include/rapidjson/document.h"
#include <algorithm>
#include <iostream>
#include <cmath>

class EyeTrackProcessor : public Processor {
  public:
    cv::Mat operator()(cv::Mat);
    EyeTrackProcessor();
    virtual ~EyeTrackProcessor(){};
    void init(const char*); // Add image to paste
    std::string getParameters();
  private:
    void drawBorder( cv::Mat );
    void calcGradient( cv::Mat );
    cv::Rect getRightEyeRoI( cv::Mat);
    cv::Rect getLeftEyeRoI( cv::Mat);
    static void createOutput(cv::Mat&, cv::Mat&);
    // Helper function for debugging
    static void printDim( const std::string&, const cv::Mat&);
    std::unique_ptr<FaceDetectorProcessor> innerFaceDetector;
    // Parameters for eye location in units of face width,height
    double eyeWidth = 0.3;
    double eyeHeight = 0.3;
    double eyeX0 = 0.15;
    double eyeY0 = 0.25;
    int useQuantile = 0;
    double gradThreshQuantile = 0.75;
    double gradThreshStdScale = 0.5;
    int ksize = CV_SCHARR;
};

EyeTrackProcessor::EyeTrackProcessor() {
  // Use inner FaceDetector processor for finding faces in image
  innerFaceDetector = std::unique_ptr<FaceDetectorProcessor>{new FaceDetectorProcessor()};
}

void EyeTrackProcessor::init( const char* json ) {
  using namespace rapidjson;
  Document document;

  document.Parse(json);
  assert(document.IsObject());
  assert(document.HasMember("useQuantile"));
  useQuantile = document["useQuantile"].GetInt();
  gradThreshQuantile = document["qscale"].GetDouble();
  gradThreshStdScale = document["sscale"].GetDouble();
  ksize = document["ksize"].GetInt();
}

std::string EyeTrackProcessor::getParameters() {
  std::ostringstream os;
  os <<  "{\"useQuantile\":" << useQuantile << "," << 
         "\"qscale\":" << gradThreshQuantile << "," <<
         "\"sscale\":" << gradThreshStdScale << "," <<
         "\"ksize\":" << ksize << "}";
  return os.str();
}

void EyeTrackProcessor::printDim(const std::string& name, const cv::Mat& im) {
  std::cout << name << ": " << im.rows << "x" << im.cols 
            << "x" << im.channels() << std::endl;
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
  cv::Mat grad(grey.size(), CV_8UC2);
  std::vector<cv::Mat> gradV(2);
  cv::Sobel(grey, gradV[0], ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
  cv::Sobel(grey, gradV[1], ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
  cv::merge( gradV, grad); 

  // Normalized gradient - need to convert to float
  cv::Mat nGrad;
  grad.convertTo(nGrad, CV_32FC1);
  cv::normalize(nGrad, nGrad, 1);
  printDim( "grad", grad);
  printDim( "nGrad", nGrad);
  std::cout << cv::norm( nGrad ) << std::endl;
  /*
  cv::Mat nGradNorm; // Check: norm of elements should sum to 1
  nGrad.copyTo( nGradNorm );
  nGradNorm = nGradNorm.reshape(1, nGrad.rows*nGrad.cols);
  cv::pow( nGradNorm, 2, nGradNorm);
  cv::Mat nGradNormSum;
  cv::reduce(nGradNorm, nGradNormSum, 1, CV_REDUCE_SUM);
  cv::sqrt( nGradNormSum, nGradNormSum);
  nGradNormSum = nGradNormSum.reshape(0, nGrad.rows);
  std::cout << nGradNormSum << std::endl;
  */ 

  // Convert to 8U1 
  cv::Mat absGrad;
  cv::convertScaleAbs(grad, absGrad);
  cv::Mat gradient;
  gradient = absGrad.reshape(1, grad.rows*grad.cols);

  cv::reduce(gradient, gradient, 1, CV_REDUCE_AVG, -1);
  // cv::addWeighted(absGrad.col(0), 0.5, absGrad.col(1), 0.5, 0, gradient);
  gradient = gradient.reshape(0, grad.rows);
  // cv::addWeighted(absGradX, 0.5, absGradY, 0.5, 0, gradient);
  std::cout << "Thresholding" << std::endl;
  std::cout << gradient.rows << "x" << gradient.cols << std::endl;
  // std::cout << gradient << std::endl;
  if ( useQuantile == 1 ) {
    // Threshold by quantile
    std::vector<int> gradVals;
    gradient.reshape(0,1).copyTo(gradVals);
    std::sort(gradVals.begin(), gradVals.end());
    int gradThresh = gradVals[std::floor(gradThreshQuantile*gradVals.size())];
    cv::threshold( gradient, gradient, gradThresh, 255, cv::THRESH_TOZERO);
  } else {
    // Threshold to remove small gradient values 
    cv::Scalar gradMean;
    cv::Scalar gradStd;
    cv::meanStdDev( gradient, gradMean, gradStd);
    cv::Scalar gradThreshold;
    cv::addWeighted(gradMean,1.0, gradStd, gradThreshStdScale, 0, gradThreshold);
    cv::threshold( gradient, gradient, gradThreshold.val[0], 255, cv::THRESH_TOZERO);
  }

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
