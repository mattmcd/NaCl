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
    void drawGradient( cv::Mat );
    cv::Mat calcGradient( const cv::Mat& );
    cv::Mat calcGradientMagnitude( const cv::Mat& );
    cv::Mat thresholdGradient( cv::Mat );
    cv::Mat calcObjective( cv::Mat );
    cv::Rect getRightEyeRoI( const cv::Mat&);
    cv::Rect getLeftEyeRoI( const cv::Mat&);
    static void createOutput(cv::Mat&, cv::Mat&);
    // Helper function for debugging
    static void printDim( const std::string&, const cv::Mat&);
    std::unique_ptr<FaceDetectorProcessor> innerFaceDetector;
    // Parameters for eye location in units of face width,height
    double eyeWidth = 0.2;
    double eyeHeight = 0.2;
    double eyeX0 = 0.2;
    double eyeY0 = 0.3;
    int useQuantile = 1;
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

    drawGradient(imFace(rightEye));
    drawGradient(imFace(leftEye));
    
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

void EyeTrackProcessor::drawGradient( cv::Mat im ) {
  try {

    // Calculate mean dot product of displacement vector and gradient
    cv::Mat objective;
    objective = calcObjective( im );
    
    
    double maxVal;
    cv::Point maxLoc;
    cv::Mat objectiveAbs;
    cv::minMaxLoc( objective, NULL, &maxVal, NULL, &maxLoc);

    //cv::Mat dest( objective.size(), CV_8UC3);

    //cv::cvtColor( objective, dest, CV_GRAY2BGR);
    //dest.copyTo(im);
    cv::circle(im, maxLoc, 3, cv::Scalar(0,255,0), -1);
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
  }
}

cv::Mat EyeTrackProcessor::calcObjective( cv::Mat im ) {
  // Calculate gradient as double 
  cv::Mat grad;
  grad = calcGradient( im );
  
  // Calculate magnitude of gradient as double
  cv::Mat gradMag;
  gradMag = calcGradientMagnitude( grad );

  // Convert to 8UC1 and threshold
  cv::Mat gradient;
  gradient = thresholdGradient( gradMag );
  // return gradient;

  int nRows = grad.rows;
  int nCols = grad.cols;
  int nEl   = nRows * nCols;
  grad = grad.reshape(1, nEl );
  gradMag = gradMag.reshape( 1, nEl );
  gradient = gradient.reshape( 1, nEl );
  std::vector<cv::Point2d> gradVect;
  for (int i=0; i<grad.rows; i++ ) {
    if (  gradient.at<double>(i) == 0 ) {
      // Zero gradient
      gradVect.push_back( cv::Point2d(0, 0));
    } else {
      // Scale to unit vector
      gradVect.push_back( cv::Point2d( grad.at<double>(i,0), grad.at<double>(i,1))
        * (1/gradMag.at<double>(i)));
    }
  }

  std::vector<cv::Point2d> dispVect;
  for (int i=0; i<nRows; i++ ) {
    for (int j=0; j<nCols; j++) {
      dispVect.push_back( cv::Point2d(j,i));
    }
  }

  cv::Mat imC;
  im.copyTo(imC);
  cv::GaussianBlur(imC, imC, cv::Size(3,3), 0,0, cv::BORDER_DEFAULT);
  cv::Mat grey(imC.size(), CV_8UC1);
  cv::cvtColor( imC, grey, CV_BGR2GRAY);
  grey = grey.reshape( 1, nEl);

  cv::Mat objective(nRows, nCols, CV_64FC1);
  for (int i=0; i<nRows; i++ ) {
    for (int j=0; j<nCols; j++) {
      cv::Point2d c(j,i);
      double obj = 0;
      for (int k=0; k<nEl; k++) {
        if ( gradient.at<double>(k) == 0) continue; 
        cv::Point2d disp;
        disp = dispVect[k] - c;
        double normVal = cv::norm( disp );
        double objVal = disp.dot(gradVect[k]) * (1/normVal) *
          (255-grey.at<char>(k));
        if ( objVal > 0 ) {
          obj += objVal*objVal;
        }
      }
      objective.at<double>(i, j) = obj/nEl;
    }
  }
  // objective = objective.reshape(1, nRows);

  double maxVal;
  cv::Point maxLoc;
  cv::Mat objectiveAbs;
  cv::minMaxLoc( objective, NULL, &maxVal, NULL, &maxLoc);

  cv::convertScaleAbs( objective*(255/maxVal), objectiveAbs );
  return objectiveAbs;
}

cv::Mat EyeTrackProcessor::calcGradient( const cv::Mat& im) {
  // Takes input BGR image and calculates gradient after smoothing
  // Remove noise by blurring with Gaussian
  
  cv::Mat imC;
  im.copyTo(imC);
  cv::GaussianBlur(imC, imC, cv::Size(3,3), 0,0, cv::BORDER_DEFAULT);
  cv::Mat grey(imC.size(), CV_8UC1);
  cv::cvtColor( imC, grey, CV_BGR2GRAY);
  
  int scale = 1;
  int delta = 0;
  int ddepth = CV_64F;
  cv::Mat grad(grey.size(), CV_64FC2);
  std::vector<cv::Mat> gradV(2);
  cv::Sobel(grey, gradV[0], ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
  cv::Sobel(grey, gradV[1], ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
  cv::merge( gradV, grad); 

  return grad;
}

cv::Mat EyeTrackProcessor::calcGradientMagnitude( const cv::Mat& im ) {
  
  // Convert gradient to vector of points
  cv::Mat grad;
  grad = im.reshape(1, im.rows*im.cols);
  cv::Mat gradMag(grad.rows, 1, CV_64FC1);
  std::vector<cv::Point2d> gradPts;
  for ( int i=0; i<grad.rows; i++) {
    gradPts.push_back(cv::Point2d(grad.at<double>(i,0), grad.at<double>(i,1)));
  }
  for ( int i=0; i<gradPts.size(); ++i ) {
    auto normVal = cv::norm(gradPts[i]);
    // gradPt *= (normVal == 0) ? 0 : 1/cv::norm(gradPt);
    gradMag.at<double>(i) = normVal;
  }
  gradMag = gradMag.reshape( 0, im.rows );
  return gradMag;
}

cv::Mat EyeTrackProcessor::thresholdGradient( cv::Mat gradMag ) {
  cv::Mat gradient;
  double maxVal;
  cv::minMaxLoc( gradMag, NULL, &maxVal);
  cv::convertScaleAbs( gradMag*(255/maxVal), gradient );
  
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
  return gradient;
}

cv::Rect EyeTrackProcessor::getRightEyeRoI( const cv::Mat& imFace ) {
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

cv::Rect EyeTrackProcessor::getLeftEyeRoI( const cv::Mat& imFace ) {
    int faceWidth = imFace.cols;
    int w = std::round( faceWidth * eyeWidth);
    int x0 = std::round( faceWidth * eyeX0);
    // Person right eye (camera left)
    cv::Rect roiLeftEye = getRightEyeRoI(imFace);
    roiLeftEye += cv::Point(faceWidth - 2*x0 - w, 0);
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
  auto scProcReg = ProcessorRegister<
    EyeTrackProcessor>("Eye Tracker (experimental)");
}
