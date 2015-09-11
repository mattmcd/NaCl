#ifndef PROCESSOR_FACEDETECT_HPP
#define PROCESSOR_FACEDETECT_HPP
#include "face_detector.hpp"

class FaceDetectorProcessor : public FaceDetector {
  public:
    cv::Mat operator()(cv::Mat);
    FaceDetectorProcessor();
    virtual ~FaceDetectorProcessor(){};
    // Helper method to detect faces
    virtual void detectFaces(const cv::Mat, std::vector<cv::Rect>&);
  private:
    cv::CascadeClassifier face_cascade;
    bool xmlLoaded;
    static void createOutput(cv::Mat&, cv::Mat&);
    static std::string getClassifier();
};

#endif
