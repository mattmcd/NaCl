#ifndef IMAGE_PROC_HPP
#define IMAGE_PROC_HPP

#include <opencv2/core/core.hpp>

class ImageProc {
  public:
    explicit ImageProc() {
    };
    virtual ~ImageProc() {};
    cv::Mat process( std::function<cv::Mat(cv::Mat)> const& f, cv::Mat im ) {
      return f(im);
    };
    cv::Mat process(cv::Mat im) {
      auto f = [](cv::Mat x){ return x; };
      return process(f,im);
    }    
};

#endif
