#ifndef FACE_DETECTOR_HPP 
#define FACE_DETECTOR_HPP

#include "processor.hpp"

class FaceDetector : public Processor {
  public:
    virtual void detectFaces(const cv::Mat, std::vector<cv::Rect>&)=0;
};

#endif
