#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <opencv2/core/core.hpp>
#include <string>

class Processor {
  public:
    virtual cv::Mat operator()(cv::Mat)=0;
    virtual ~Processor() {}
    virtual void init( cv::Mat ) {};
    virtual void init( const char* ) {};
    virtual std::string getParameters() { return ""; };
};

#endif
