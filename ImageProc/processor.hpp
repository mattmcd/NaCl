#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <opencv2/core/core.hpp>

class Processor {
  public:
    virtual cv::Mat operator()(cv::Mat)=0;
    virtual ~Processor() {}
};

#endif
