#include "singleton_factory.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class PulseDetectionProcessor : public Processor {
  public:
    PulseDetectionProcessor() : 
      level(4), r1(0.4), r2(0.05) // , alpha(50.0), chromAttenuation(1.0) 
      {};
    cv::Mat operator()(cv::Mat);
  private:
    int level;
    double r1;
    double r2;
 //   double alpha;
 //   double chromAttenuation;
    cv::Mat prevFrame1;
    cv::Mat prevFrame2;
};

cv::Mat PulseDetectionProcessor::operator()(cv::Mat im) {
  cv::Mat dest;
  // Convert to YCrCb color space
//  cv::cvtColor( im, dest, CV_YCrCb2RGB);

  // Gaussian blur using image pyramid
  for (auto i=0; i<level; i++) {
    cv::pyrDown(im,dest);
    im = dest;
  }
  // Time filtering
  if ( prevFrame1.empty() ) {
    prevFrame1 = im.clone();
  } else { 
    cv::Mat y1 = r1*im + (1.0 - r1)*prevFrame1;
    prevFrame1 = y1.clone();
  }

  if ( prevFrame1.empty() ) {
    prevFrame2 = im.clone();
  } else { 
    cv::Mat y2 = r2*im + (1.0 - r2)*prevFrame2;
    prevFrame2 = y2.clone();
  }
  cv::subtract(prevFrame1,prevFrame2,im);

  for (auto i=0; i<level; i++) {
    cv::pyrUp(im,dest);
    im = dest;
  }
  // Set alpha channel
  cv::add( cv::Scalar(0,0,0,255), dest, im );
  return im;
}

namespace {
  auto diffProcReg = ProcessorRegister<PulseDetectionProcessor>("Pulse Detection");
}

