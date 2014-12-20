#include "singleton_factory.hpp"
#include <iostream>
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main(int argc, char* argv[])
{

  auto fname = "mmcdonne.jpg";
  if (argc > 1) {
    fname = argv[1];
  }
  cv::Mat input;
  input = cv::imread( fname, CV_LOAD_IMAGE_COLOR );
  if (!input.data) {
    std::cout << "Could not read image " << fname << std::endl;
    return -1;
  }
  // Processors expect 8UC4
  cv::Mat im;
  cv::cvtColor( input, im, CV_BGR2RGBA );
  
  // Use first processor
  auto processorFactory = SingletonFactory<std::function<std::unique_ptr<Processor>()>>::getInstance();
  auto names = processorFactory.getNames();
  auto processor = processorFactory.getObject( names[0] )();
  auto output = (*processor)( im );
  
  cv::cvtColor( output, output, CV_RGBA2BGR );
  
  cv::namedWindow( "Input", CV_WINDOW_AUTOSIZE );
  cv::namedWindow( "Processed", CV_WINDOW_AUTOSIZE );
  cv::imshow( "Input", input);
  cv::imshow( "Processed", output);
  cv::waitKey();

  return 0;
}
